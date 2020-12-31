#include <utility>
#include <tuple>

#include "dinterr.h"
#include "proto.h"
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

using DinterrSerialData = \
std::tuple<
  uLong,
  unsigned char,
  dinterr_pos_t,
  dinterr_count_t,
  dinterr_pid_t,
  unsigned int,
  unsigned int,
  dinterr_pos_t,
  dinterr_ts_t
>;

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

        /* Dinterr data to be serialized for transport
         * over a network
         */
        uLong           _crc;  // uLong from zlib.h
        unsigned char   _attrs;
        dinterr_pos_t   _pos;
        dinterr_count_t _count;
        dinterr_pid_t   _pid;
        dinterr_ts_t    _timestamp;
        unsigned int    _ra_page_count;
        unsigned int    _ra_cache_misses;
        dinterr_pos_t   _ra_last_cache_pos;

    public:
        DinterrSerialData
        serialize() && {
            return DinterrSerialData(
                       _crc,
                       _attrs,
                       _pos,
                       _count,
                       _pid,
                       _ra_page_count,
                       _ra_cache_misses,
                       _ra_last_cache_pos,
                       _timestamp);
        }

        void calc_crc(void) {
            uLong tcrc;

            tcrc = crc32(0L, Z_NULL, 0);
            tcrc = crc32(tcrc, (const Bytef*)&_attrs, sizeof(_attrs));
            tcrc = crc32(tcrc, (const Bytef*)&_pos, sizeof(_pos));
            tcrc = crc32(tcrc, (const Bytef*)&_count, sizeof(_count));
            tcrc = crc32(tcrc, (const Bytef*)&_pid, sizeof(_pid));

            if (is_attr_set(&_attrs, DINTERR_ATTR_READAHEAD)) {
                tcrc = crc32(tcrc, (const Bytef*)&_ra_page_count, sizeof(_ra_page_count));
                tcrc = crc32(tcrc, (const Bytef*)&_ra_cache_misses, sizeof(_ra_cache_misses));
            }

            tcrc = crc32(tcrc, (const Bytef*)&_ra_last_cache_pos, sizeof(_ra_last_cache_pos));
            tcrc = crc32(tcrc, (const Bytef*)&_timestamp.tv_sec, sizeof(_timestamp.tv_sec));
            tcrc = crc32(tcrc, (const Bytef*)&_timestamp.tv_usec, sizeof(_timestamp.tv_usec));
            _crc = std::move(tcrc);
        }

        /*
         * accessor methods for checking once in CHECKSUM state.
         * mainly used for debugging calc_crc() function calls.
         */
        uLong get_crc(void) const {
            return _crc;
        }
        unsigned char get_attrs(void) const {
            return _attrs;
        }
        dinterr_pos_t get_pos(void) const {
            return _pos;
        }
        dinterr_count_t get_count(void) const {
            return _count;
        }
        dinterr_pid_t get_pid(void) const {
            return _pid;
        }
        unsigned int get_ra_page_count(void) const {
            return _ra_page_count;
        }
        unsigned int get_ra_cache_misses(void) const {
            return _ra_cache_misses;
        }
        dinterr_pos_t get_ra_last_cache_pos(void) const {
            return _ra_last_cache_pos;
        }
        long int get_ts_sec(void) const {
            return _timestamp.tv_sec;
        }
        long int get_ts_usec(void) const {
            return _timestamp.tv_usec;
        }

        /*
         * modifier methods setting private data via typestates fsm
         */
        void add_attrs(unsigned char attrs) {
            _attrs = std::move(attrs);
        }

        void add_pos(dinterr_pos_t pos) {
            _pos = std::move(pos);
        }

        void add_count(dinterr_count_t count) {
            _count = std::move(count);
        }

        void add_pid(dinterr_pid_t pid) {
            _pid = std::move(pid);
        }

        void add_timestamp(dinterr_ts_t *timestamp) {
            _timestamp = std::move(*timestamp);
        }

        void add_ra_page_count(unsigned int ra_page_count) {
            _ra_page_count = std::move(ra_page_count);
        }

        void add_ra_cache_misses(unsigned int ra_cache_misses) {
            _ra_cache_misses = std::move(ra_cache_misses);
        }

        void add_ra_last_cache_pos(dinterr_pos_t ra_last_cache_pos) {
            _ra_last_cache_pos = std::move(ra_last_cache_pos);
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
    ValidQuery<DinterrRecordState::CHECKSUM, &DinterrData::get_attrs>,
    ValidQuery<DinterrRecordState::CHECKSUM, &DinterrData::get_pos>,
    ValidQuery<DinterrRecordState::CHECKSUM, &DinterrData::get_count>,
    ValidQuery<DinterrRecordState::CHECKSUM, &DinterrData::get_pid>,
    ValidQuery<DinterrRecordState::CHECKSUM, &DinterrData::get_ra_page_count>,
    ValidQuery<DinterrRecordState::CHECKSUM, &DinterrData::get_ra_cache_misses>,
    ValidQuery<DinterrRecordState::CHECKSUM, &DinterrData::get_ra_last_cache_pos>,
    ValidQuery<DinterrRecordState::CHECKSUM, &DinterrData::get_ts_sec>,
    ValidQuery<DinterrRecordState::CHECKSUM, &DinterrData::get_ts_usec>
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
PROTENC_DECLARE_QUERY_METHOD(get_attrs);
PROTENC_DECLARE_QUERY_METHOD(get_pos);
PROTENC_DECLARE_QUERY_METHOD(get_count);
PROTENC_DECLARE_QUERY_METHOD(get_pid);
PROTENC_DECLARE_QUERY_METHOD(get_ra_page_count);
PROTENC_DECLARE_QUERY_METHOD(get_ra_cache_misses);
PROTENC_DECLARE_QUERY_METHOD(get_ra_last_cache_pos);
PROTENC_DECLARE_QUERY_METHOD(get_ts_sec);
PROTENC_DECLARE_QUERY_METHOD(get_ts_usec);

PROTENC_END_WRAPPER;


static DinterrRecordBuilder<DinterrRecordState::INIT>
dinterrRecord() {
    return {};
}
