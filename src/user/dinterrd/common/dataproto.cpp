#include <utility>
#include <tuple>

#include "dataproto.h"
#include "serdes.h"
#include "protenc.h"

/* Dinterr Serialized Data Protocol implementation:
 * pattern and wrapper functionality via
 * ProtEnc Github project's "protenc.h"
 *
 * Reference:
 * https://github.com/nitnelave/ProtEnc
 *
 * (requires -std=c++2a (i.e. c++ 20 standard))
 */

using DinterrSerialized = std::tuple<dinterr_data_t*>;

/* forward reference used as a friend class
 * to gain access for instantiation of 
 * templated DinterrData objects
 */
template <DinterrRecordState>
class DinterrRecordBuilder;

class DinterrData {
    private:
        /* constructor is private so it can't be instantiated
 	 * by anything unless explicitly allowed via a friendship.
 	 *
         * We make DinterrRecordBuilder<DinterrRecordState> class
         * a friend allowing it to do so using "protenc.h"
         * macro templating.
         *
         * We will enforce that only a DinterrRecordBuilder
         * in the INIT state can instantiate a DinterrData object.
         *
         * DinterrData objects subsequently go through "states"
         * towards data serialization via return "typestate" templates.
         */
        DinterrData() = default;
        template <DinterrRecordState>
        friend class ::DinterrRecordBuilder;

        /* Dinterr data that will be populated from
         * modified inotify fsnotify_access events
         */
        dinterr_data_t _data;

        /* DinterrSerdesData object reference
         * for serialization after crc is computed.
         */
       DinterrSerdesData *_serdes;

    public:
        DinterrSerialized
        serialize() && {
            return DinterrSerialized();
        }

        void calc_crc(void) {
            uLong tcrc;

            tcrc = crc32(0L, Z_NULL, 0);

            tcrc = crc32(tcrc, (const Bytef*)&_data._attrs,
                           sizeof(_data._attrs));
            tcrc = crc32(tcrc, (const Bytef*)&_data._pos,
                           sizeof(_data._pos));
            tcrc = crc32(tcrc, (const Bytef*)&_data._count,
                           sizeof(_data._count));
            tcrc = crc32(tcrc, (const Bytef*)&_data._pid,
                           sizeof(_data._pid));

            if (is_attr_set(&_data._attrs, DINTERR_ATTR_READAHEAD)) {
                tcrc = crc32(tcrc, (const Bytef*)&_data._ra_page_count,
                               sizeof(_data._ra_page_count));
                tcrc = crc32(tcrc, (const Bytef*)&_data._ra_cache_misses,
                               sizeof(_data._ra_cache_misses));
            }

            tcrc = crc32(tcrc, (const Bytef*)&_data._ra_last_cache_pos,
                           sizeof(_data._ra_last_cache_pos));
            tcrc = crc32(tcrc, (const Bytef*)&_data._timestamp.tv_sec,
                           sizeof(_data._timestamp.tv_sec));
            tcrc = crc32(tcrc, (const Bytef*)&_data._timestamp.tv_usec,
                           sizeof(_data._timestamp.tv_usec));

            _data._crc = std::move(tcrc);

            /* we have _crc, great, now serialize dinterr_data_t
             * member struct and return a new instance of object.
             *
             * Note: important to scope this such that this instance
             *       will be destroyed and reclaim back memory that
             *       DinterrSerdesData objects allocate. The default
             *       destructor will take care of freeing memory.
             */
            _serdes = new DinterrSerdesData(&_data);
        }

        /*
         * accessor methods for checking once in CHECKSUM state.
         * mainly used for debugging calc_crc() function calls.
         * add more methods if needed, however, methods must be
         * updated via a PROTENC_DECLARE_QUERY_METHOD and ValidQuery
         * per method.
         */
        uLong get_crc(void) const {
            return _data._crc;
        }
        DinterrSerdesData* get_serdes(void) const {
            return _serdes;
        }

        /*
         * modifier methods setting private data via typestates fsm
         */
        void add_attrs(unsigned char attrs) {
            _data._attrs = std::move(attrs);
        }

        void add_pos(dinterr_pos_t pos) {
            _data._pos = std::move(pos);
        }

        void add_count(dinterr_count_t count) {
            _data._count = std::move(count);
        }

        void add_pid(dinterr_pid_t pid) {
            _data._pid = std::move(pid);
        }

        void add_timestamp(dinterr_ts_t *timestamp) {
            _data._timestamp = std::move(*timestamp);
        }

        void add_ra_page_count(unsigned int ra_page_count) {
            _data._ra_page_count = std::move(ra_page_count);
        }

        void add_ra_cache_misses(unsigned int ra_cache_misses) {
            _data._ra_cache_misses = std::move(ra_cache_misses);
        }

        void add_ra_last_cache_pos(dinterr_pos_t ra_last_cache_pos) {
            _data._ra_last_cache_pos = std::move(ra_last_cache_pos);
        }
};

using prot_enc::Transitions;
using prot_enc::Transition;
using prot_enc::FinalTransitions;
using prot_enc::FinalTransition;
using prot_enc::ValidQueries;
using prot_enc::ValidQuery;
using prot_enc::InitialStates;

using MyInitialStates = InitialStates<DinterrRecordState::INIT>;
using MyTransitions = Transitions<
    Transition<DinterrRecordState::INIT,
               DinterrRecordState::ATTRS,
               &DinterrData::add_attrs>,
    Transition<DinterrRecordState::ATTRS,
               DinterrRecordState::POS,
               &DinterrData::add_pos>,
    Transition<DinterrRecordState::POS,
               DinterrRecordState::COUNT,
               &DinterrData::add_count>,
    Transition<DinterrRecordState::COUNT,
               DinterrRecordState::PID,
               &DinterrData::add_pid>,
    Transition<DinterrRecordState::PID,
               DinterrRecordState::RA_PAGE_COUNT,
               &DinterrData::add_ra_page_count>,
    Transition<DinterrRecordState::RA_PAGE_COUNT,
               DinterrRecordState::RA_CACHE_MISSES,
               &DinterrData::add_ra_cache_misses>,
    Transition<DinterrRecordState::RA_CACHE_MISSES,
               DinterrRecordState::RA_LAST_CACHED_POS,
               &DinterrData::add_ra_last_cache_pos>,
    Transition<DinterrRecordState::RA_LAST_CACHED_POS,
               DinterrRecordState::TIMESTAMP,
               &DinterrData::add_timestamp>,
    Transition<DinterrRecordState::TIMESTAMP,
               DinterrRecordState::CHECKSUM,
               &DinterrData::calc_crc>
>;
using MyFinalTransitions = FinalTransitions<
    FinalTransition<DinterrRecordState::CHECKSUM, &DinterrData::serialize>
>;
using MyValidQueries = ValidQueries<
    ValidQuery<DinterrRecordState::CHECKSUM, &DinterrData::get_crc>,
    ValidQuery<DinterrRecordState::CHECKSUM, &DinterrData::get_serdes>
>;
PROTENC_START_WRAPPER(DinterrRecordBuilder, DinterrData, DinterrRecordState,
                      MyInitialStates, MyTransitions, MyFinalTransitions,
                      MyValidQueries);

PROTENC_DECLARE_TRANSITION(add_attrs);
PROTENC_DECLARE_TRANSITION(add_pos);
PROTENC_DECLARE_TRANSITION(add_count);
PROTENC_DECLARE_TRANSITION(add_pid);
PROTENC_DECLARE_TRANSITION(add_ra_page_count);
PROTENC_DECLARE_TRANSITION(add_ra_cache_misses);
PROTENC_DECLARE_TRANSITION(add_ra_last_cache_pos);
PROTENC_DECLARE_TRANSITION(add_timestamp);
PROTENC_DECLARE_TRANSITION(calc_crc);

PROTENC_DECLARE_FINAL_TRANSITION(serialize);

PROTENC_DECLARE_QUERY_METHOD(get_crc);
PROTENC_DECLARE_QUERY_METHOD(get_serdes);

PROTENC_END_WRAPPER;


static DinterrRecordBuilder<DinterrRecordState::INIT>
dinterrRecord() {
    return {};
}
