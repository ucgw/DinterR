#ifndef _NETPROTO_H_
#define _NETPROTO_H_

#include "sml.hpp"

enum class DinterrTransportState {
  INIT,
  LOAD,
  LOAD_ACK,
  LOAD_FAIL,
  CTS,
  AGG,
  REC,
  DATA_RETRY,
  DATA_FIN,
  DATA_FINACK,
  LOAD_FIN,
  LOAD_FINACK
};

#endif  // _NETPROTO_H_
