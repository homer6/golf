
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <iterator>
#include <future>
#include <functional>
#include <stdexcept>
#include <thread>
#include <memory>
using namespace std;

typedef unsigned char char_type;
typedef map<char_type,int> count_map;
typedef unique_ptr<char[]> unique_prt_char_array;


struct Buffer{

    Buffer( unsigned long size )
        :size(size), ptr( new char[size] )
    {

    }

    Buffer( Buffer&& original )
        :size( original.size ), ptr( std::move(original.ptr) )
    {

    }

    Buffer& operator=( Buffer&& original ){

        this->size = original.size;
        this->ptr = std::move( original.ptr );

        return *this;

    }


    unsigned long size;
    unique_prt_char_array ptr;

};


class Worker{

    public:

        Worker( string filename, unsigned long start_index = 0, long end_index = 0 )
            :input_file(filename,ios::binary), filename(filename), start_index(start_index), end_index(end_index)
        {

            //end_index is one-past the last valid index
            cout << ++Worker::thread_count << endl;
            //cout << start_index << endl;


        }




        ~Worker(){

            this->input_file.close();

        }



        count_map operator()(){

            constexpr unsigned int max_buffer_size = 1 << 24; //16M


            if( this->end_index == 0 ){
                this->input_file.seekg( 0, this->input_file.end );
                this->end_index = this->input_file.tellg();
                this->input_file.seekg( 0, this->input_file.beg );
            }

            unsigned long span = this->end_index - this->start_index;
            cout << "span: " << span << endl;


            if( span < max_buffer_size ){

                Buffer buffer( span );

                this->input_file.seekg( this->start_index );

                input_file.readsome( buffer.ptr.get(), span );

                if( !input_file.fail() ){

                    size_t bytes_read = input_file.gcount();

                    if( bytes_read > 0 ){

                        this->count( std::move(buffer) );

                    }else{
                        throw std::runtime_error( "Zero bytes read." );
                    }

                }else{
                    throw std::runtime_error( "Error reading input file." );
                }

            }else{

                unsigned long boundary_index = (span / 2) + this->start_index;

                this->input_file.seekg( boundary_index );
                char_type first_char = this->input_file.get();
                char_type current_char = first_char;
                ++boundary_index;

                if( !this->input_file.good() ){
                    cerr << "Error reading input file." << endl;
                    //throw std::runtime_error( "Error reading input file." );
                    return this->counts;
                }

                while( current_char == first_char ){

                    current_char = this->input_file.get();
                    ++boundary_index;

                }

                Worker worker1( this->filename, this->start_index, boundary_index - 1 );
                auto future1 = async( std::launch::async, [&worker1](){ return worker1(); } );

                cout << "task 1: " << this->start_index << endl;

                Worker worker2( this->filename, boundary_index - 1, this->end_index );
                auto future2 = async( std::launch::async, [&worker2](){ return worker2(); } );

                cout << "task 2: " << boundary_index - 1 << endl;

                this->counts = this->mergeCountMaps( future1.get(), future2.get() );

                cout << "-----" << endl;

            }

            return std::move(this->counts);

        }



        void count( Buffer &&characters ){

            char_type previous_char = -1;
            int current_count = 0;

            unsigned long num_characters = characters.size;

            for( unsigned long x = 0; x < num_characters; ++x ){

                char_type current_char = characters.ptr.get()[ x ];

                if( !this->counts.count(current_char) ){

                    this->counts[current_char] = 1;
                    current_count = 1;

                }else{

                    if( previous_char == current_char ){
                        current_count++;
                        if( this->counts[current_char] < current_count ){
                            this->counts[current_char] = current_count;
                        }
                    }else{
                        current_count = 1;
                    }

                }

                previous_char = current_char;

            }

        }



        count_map mergeCountMaps( count_map&& left, count_map&& right ){

            for( auto b : right ){

                count_map::iterator left_it = left.find( b.first );

                if( left_it == left.end() ){

                    //not found, just keep right's version
                    left[ b.first ] = left_it->second;

                }else{

                    //found, keep whichever is greater
                    if( b.second > left_it->second ){
                        left[ b.first ] = b.second;
                    }

                }

            }

            return left;

        }




        friend std::ostream & operator<<( std::ostream &os, const Worker& worker );

        count_map counts;

        static int thread_count;

    private:
        ifstream input_file;
        string filename;
        unsigned long start_index;
        unsigned long end_index;


};

int Worker::thread_count = 0;



std::ostream& operator<<( std::ostream &os, Worker& worker ){

    for( auto current_pair : worker.counts ){

        os << current_pair.first;

        for( int i = 0; i < current_pair.second; ++i ){

            os << u8"¦";

        }

        os << endl;

    }

    return os;

}






int main( int argc, char **argv ){

    if( argc < 2 ){
        cerr << "Input required." << endl;
        return 1;
    }

    Worker worker( argv[1] );
    count_map counts = worker();

    worker.counts = std::move( counts );

    cout << worker;

    cout << "Thread count: " << Worker::thread_count << endl;


    return 0;

}
