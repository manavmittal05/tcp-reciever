#include "tcp_receiver.hh"

#include <algorithm>


using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    const TCPHeader head = seg.header();
    // ...

    // --- Hint ------
        // convert the seqno into absolute seqno
    uint64_t checkpoint = _reassembler.ack_index();
    uint64_t abs_seqno = unwrap(head.seqno, _isn, checkpoint);
    uint64_t stream_idx = abs_seqno - _synReceived;
    // --- Hint ------  

    // ... 
    if (head.syn && _synReceived){
        return;
    }    
    if (head.syn){
        _isn = head.seqno;
        _synReceived = true;
    }
    if(_synReceived){
        if(head.fin){
            _finReceived = true;
        }
        _reassembler.push_substring(seg.payload().copy(), stream_idx, _finReceived);

        uint64_t expected_ack = _reassembler.ack_index() + _synReceived + _finReceived;
        _ack_to_snd  = wrap(expected_ack, _isn);

    }
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if(_synReceived){
        return _ack_to_snd;
    }
    return {};
}

size_t TCPReceiver::window_size() const { return _capacity - _reassembler.stream_out().buffer_size(); }
