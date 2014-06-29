
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

    //std::default_random_engine e1( rd() );
    //std::uniform_int_distribution<int> distribution( 0, length );

    std::mt19937 e1( rd() );
    std::poisson_distribution<int> distribution( length / 2 );

    for( int x = 0; x < number_of_values; ++x ){

        int i = distribution( e1 );
        if( i < 0 ){
            cout << possible_characters[ 0 ];
        }else if( i >= length ){
            cout << possible_characters[ length - 1 ];
        }else{
            cout << possible_characters[ i ];
        }

    }

    return 0;

}
