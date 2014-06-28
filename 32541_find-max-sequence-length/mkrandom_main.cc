
#include <iostream>
#include <string>
#include <random>

using namespace std;


int main( int argc, char **argv ){

    if( argc < 2 ){
        cerr << "Input required." << endl;
        return 1;
    }

    string possible_characters( "abcdefghijklmnopqrstuvwxyz" );
    int length = possible_characters.length();

    int number_of_values = stoi( string(argv[1]) );


    std::random_device rd;
    std::default_random_engine e1( rd() );

    std::uniform_int_distribution<int> uniform_dist( 0, length );

    for( int x = 0; x < number_of_values; ++x ){

        cout << possible_characters[ uniform_dist( e1 ) ];

    }

    return 0;

}
