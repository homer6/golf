
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
using namespace std;

typedef unsigned char char_type;
typedef map<char_type,int> count_map;
typedef vector<char_type> char_vector;


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

            constexpr unsigned int buffer_size = 1 << 24; //16M


            if( this->end_index == 0 ){
                this->input_file.seekg( 0, this->input_file.end );
                this->end_index = this->input_file.tellg();
                this->input_file.seekg( 0, this->input_file.beg );
            }

            unsigned long span = this->end_index - this->start_index;
            cout << "span: " << span << endl;


            if( span < buffer_size ){

                char *buffer = new char[ buffer_size ];


                this->input_file.seekg( this->start_index );
                input_file.readsome( buffer, span );

                if( !input_file.fail() ){

                    size_t bytes_read = input_file.gcount();

                    if( bytes_read > 0 ){

                        char_vector current_vector( buffer, buffer + (bytes_read-1) );
                        delete[] buffer;

                        this->count( std::move(current_vector) );

                    }else{

                        delete[] buffer;
                        cerr << "Error reading input file." << endl;
                        //throw std::runtime_error( "Zero bytes read." );
                        return this->counts;

                    }

                }else{

                    delete[] buffer;
                    cerr << "Error reading input file." << endl;
                    //throw std::runtime_error( "Error reading input file." );
                    return this->counts;

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
                promise<count_map> promise1;
                auto future1 = promise1.get_future();
                thread thread1([]( Worker& worker, promise<count_map> &&current_promise ){
                    current_promise.set_value( std::move(worker()) );
                }, std::ref(worker1), std::move(promise1));

                cout << "task 1: " << this->start_index << endl;

                Worker worker2( this->filename, boundary_index - 1, this->end_index );
                promise<count_map> promise2;
                auto future2 = promise2.get_future();
                thread thread2([]( Worker& worker, promise<count_map> &&current_promise ){
                    current_promise.set_value( std::move(worker()) );
                }, std::ref(worker2), std::move(promise2));

                cout << "task 2: " << boundary_index - 1 << endl;

                thread1.join();
                thread2.join();

                this->counts = this->mergeCountMaps( future1.get(), future2.get() );

                cout << "-----" << endl;





                /*
                packaged_task<count_map(Worker&)> task1();
                future<count_map> result1 = task1.get_future();

                task1( worker1 );

                cout << "task 1: " << this->start_index << endl;

                Worker worker2( this->filename, boundary_index - 1, this->end_index );
                packaged_task<count_map(Worker&)> task2([]( Worker& worker ){
                    return worker();
                });
                future<count_map> result2 = task2.get_future();

                task2( worker2 );

                cout << "task 2: " << boundary_index - 1 << endl;

                this->counts = this->mergeCountMaps( std::move(result1.get()), std::move(result2.get()) );

                cout << "-----" << endl;

                */

            }

            return std::move(this->counts);

        }



        void count( char_vector characters ){

            char_type previous_char = -1;
            int current_count = 0;

            for( char_type current_char : characters ){

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
