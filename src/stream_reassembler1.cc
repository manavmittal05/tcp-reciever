#include "stream_reassembler.hh"
#include <bits/stdc++.h>

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
    :_output(capacity)
{
    buffer = vector<int>(capacity, -1);
    last_index_written = -1;
    unassembled_bytes_in_buffer = 0;
    max_index_recieved = 0;
    got_eof = false;
}


//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    
    // unassembeled_data[index] = data;

    max_index_recieved = max((unsigned long)max_index_recieved, index+data.size());

    if(buffer.size() < index+data.size()){
        buffer.resize(index+data.size(), -1);
    }

    for(int i = index; i < data.size()+index; i++){
        buffer[i] = data[i-index];
    }

    string bytes_to_write = "";
    int i = -1;
    for(i = last_index_written+1; i < max_index_recieved; i++){
        if(buffer[i] == -1) break;
        if(_output.remaining_capacity() <= bytes_to_write.size()) break;
        bytes_to_write += (char)buffer[i];
        buffer[i] = -1;
    }
    
    last_index_written = i-1;
    
    _output.write(bytes_to_write);

    int count = 0;
    for(int i = last_index_written+1; i < buffer.size(); i++){
        if(buffer[i] != -1) count += 1;
    }

    unassembled_bytes_in_buffer = count;

    if(eof){
        got_eof = true;
    }
    if(empty() && got_eof){
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const { return unassembled_bytes_in_buffer; }

bool StreamReassembler::empty() const { return unassembled_bytes_in_buffer == 0; }

size_t StreamReassembler::ack_index() const { return last_index_written+1; }