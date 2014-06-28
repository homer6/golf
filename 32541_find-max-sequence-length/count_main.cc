
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <iterator>
using namespace std;


int main( int argc, char **argv ){

    if( argc < 2 ){
        cerr << "Input required." << endl;
        return 1;
    }

    map<unsigned char,int> counts;
    vector<char> inputs;


    ifstream input_file( argv[1], ios::ate | ios::binary );
    inputs.reserve( input_file.tellg() );
    input_file.seekg( 0 );


    unsigned char current_char = input_file.get();
    unsigned char previous_char = -1;
    int current_count = 0;

    while( input_file.good() ){

        if( !counts.count(current_char) ){
            counts[current_char] = 1;
            current_count = 1;
        }else{
            if( previous_char == current_char ){
                current_count++;
                if( counts[current_char] < current_count ){
                    counts[current_char] = current_count;
                }
            }else{
                current_count = 1;
            }
        }

        previous_char = current_char;
        current_char = input_file.get();

    }

    for( auto current_pair : counts ){


        cout << current_pair.first;

        for( int i = 0; i < current_pair.second; ++i ){

            cout << u8"¦";

        }

        cout << endl;

    }


    return 0;

}
