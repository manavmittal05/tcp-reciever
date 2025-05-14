#include "stream_reassembler.hh"
#include <bits/stdc++.h>

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
    :_output(capacity)
{
    _capacity = capacity;
    buffer = vector<int>(capacity, -200); // used -200 because int(char()) ranges from -128 to 127
    eof_index = 0;
    got_eof = false;
}


//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {

    // recording if got EOF and also for which index got the EOF
    if(eof){
        got_eof = true;
        eof_index = index + data.size();
    }

    // if already got the EOF and the ack_index is equal to the eof index then all the data 
    // is in the byte_stream and we can end the input of byte_stream.
    if(got_eof && ack_index() == eof_index){
        _output.end_input();
        return;
    }

    // if get an index which is greater that currect expected index plus the buffers
    // storage capacity then the segment will be discarded.
    if(index >= ack_index() + _capacity) return;


    // placing the provided segment at the appropriate place in the buffer
    int i = 0;
    int j = index - ack_index();

    if (j < 0){
        if(index+data.size() <= ack_index()) return;
        i = ack_index() - index;
        j = 0;
        while (j < _capacity && i < data.size()){
            buffer[j++] = (int)data[i++];
        }
    }
    else{
        while(j < _capacity && i < data.size()){
            buffer[j++] = (int)data[i++];
        }
    }

    // After placing the recieved segment in the buffer we would like to push the 
    // bytes that can be pushed into the stream immediately to here I am appending
    // all the bytes that can be pushed into the stream in the str_to_push variable.

    if(buffer[0] == -200) return;
    string str_to_push = "";
    int avb_capacity = _output.remaining_capacity();

    for(int i = 0; i < _capacity && avb_capacity > 0; i++){
        if(buffer[i] == -200) break;

        str_to_push.push_back((char)buffer[i]);

        avb_capacity -= 1;
        buffer[i] = -200;
    }
    // if we have nothing to push in the byte_stream it means that we still dont have
    // the expected segement in the buffer so we will not write anything in the byte_stream
    if(str_to_push == "") return;
    // since we got some bytes to write in the byte_sream after storing those bytes
    // in str_to_push we have to shift the remaining bytes to the front of the buffer
    // to make space for more.
    else{
        vector<int> temp_buffer;
        int i = str_to_push.size();
        for(int i = str_to_push.size(); i < buffer.capacity(); i++){
            temp_buffer.push_back(buffer[i]);
            buffer[i] = -200;
        }

        for(int i = 0; i < temp_buffer.size(); i++){
            buffer[i] = temp_buffer[i];
        }

        _output.write(str_to_push);
    }


    // now if I have already recieved the EOF and my reassembler buffer is empty
    // then there no other segment to expect to we can end the input of byte_stream.
    if(got_eof && buffer_empty()){
        _output.end_input();
    }
}
// Since I have used -200 to denote the empty location in the buffer to return the amount 
// of unassembled bytes iam counting the number of positions having the value -200.
size_t StreamReassembler::unassembled_bytes() const { 
    int count = 0;
    for(auto i: buffer){
        if(i != -200) count += 1;
    }
    return count;
}

bool StreamReassembler::empty() const { return buffer_empty();}

size_t StreamReassembler::ack_index() const { return _output.bytes_written(); }

// this function return whether the buffer is empty or not by checking that are all 
// the values in the buffer are -200 or not.

bool StreamReassembler::buffer_empty() const {
    int count = 0;
    for(auto i: buffer){
        if(i == -200) count += 1;
        else break;
    }
    return count == buffer.size();
}