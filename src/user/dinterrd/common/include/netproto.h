#ifndef _NETPROTO_H_
#define _NETPROTO_H_

enum class DinterrTransportState {
  INIT,
  LOAD,
  LOAD_ACK,
  LOAD_FAIL,
  CTS,
  AGG,
  REC,
  DATA_ACK,
  DATA_RESEND,
  DATA_STOP,
  DATA_RESUME,
  DATA_FIN,
  CLOSE
};

#endif  // _NETPROTO_H_
