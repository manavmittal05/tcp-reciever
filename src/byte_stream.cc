#include "byte_stream.hh"

#include <algorithm>

// You will need to add private members to the class declaration in `byte_stream.hh`

/* Replace all the dummy definitions inside the methods in this file. */


using namespace std;

ByteStream::ByteStream(const size_t capa)
{
  capacity = capa;
  inputEnded = false;
  outputEnded = false;
  bytesWritten = 0;
  bytesRead = 0;
  _error = false;
  buffer.clear();
}

size_t ByteStream::write(const string &data) {
  // if (inputEnded) {
  //   return 0;
  // }
  int written = 0;
  while (buffer.size() < capacity && written < data.size()) {
    buffer.push_back(data[written]);
    written++;
  }
  bytesWritten += written;
  return written;
}

//! \param[in] len bytes will be copied from the output side of the buffer
std::string ByteStream::peek_output(const size_t len) const {
  // if (len > buffer.size()){
  //   this->set_error();
  // }
  string output = "";
  int peeked = 0;
  while(peeked < len && !buffer.empty() && peeked < buffer.size()) {
    output += buffer[peeked];
    peeked++;
  }
  // bytesRead += peeked;
  return output;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
  if (len > buffer.size()) set_error();
  int popped = 0;
  while(popped < len && !buffer.empty()) {
    buffer.pop_front();
    popped++;
  }
  bytesRead += popped;
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
  if (len > buffer.size()) {
    set_error();
    return "";
  }
  string output = peek_output(len);
  pop_output(len);
  return output;
}

void ByteStream::end_input() { inputEnded = true;}

bool ByteStream::input_ended() const { return inputEnded;}

size_t ByteStream::buffer_size() const {return buffer.size(); }

bool ByteStream::buffer_empty() const {return buffer.size()==0?true:false; }

bool ByteStream::eof() const { return input_ended() && buffer_empty(); }

size_t ByteStream::bytes_written() const { return bytesWritten; }

size_t ByteStream::bytes_read() const { return bytesRead;  }

size_t ByteStream::remaining_capacity() const { return capacity - buffer.size(); }

void ByteStream::push_string_inbetween(const std::string &data, const size_t index){
  buffer.insert(buffer.begin()+(index-bytesRead), data.begin(), data.end());
}

void ByteStream::replace_from_index(const int index, const string &data){
  // int data_already = buffer.size();
  buffer.erase(buffer.begin()+index, buffer.end());
  // buffer.insert((int)buffer.size(), data.begin(), data.end());
  buffer.insert(buffer.end(), data.begin(), data.end());

}
