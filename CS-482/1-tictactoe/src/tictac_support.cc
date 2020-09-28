#include <tictac_support.h>

#include <iostream>
#include <fstream>
#include <iomanip>      // std::setw

int load_board( std::string filename, int board[][3] )
{
	std::ifstream bfile( filename.c_str() );
	if( bfile.is_open() )
	{

		for( int i = 0; i < 3; i++ )
		{
			for( int j = 0; j < 3; j++ )
			{
				bfile >> board[i][j];
				//printf ("%d ", board[i][j] );
			}
			//printf ("\n");
		}

		bfile.close();
	}
	else
	{
		printf( "unable to open file: [%s]\n", filename.c_str() );
		return -1;
	}

	return 0;
}

int save_board( std::string filename, int board[][3] )
{
	std::ofstream bfile( filename.c_str() );
	if( bfile.is_open() )
	{

		for( int i = 0; i < 3; i++ )
		{
			for( int j = 0; j < 3; j++ )
			{
				bfile << std::setw(5);
				bfile << board[i][j];
				//bfile << " ";
			}
			bfile << std::endl;
		}

		bfile.close();
	}
	else
	{
		printf( "unable to open file: [%s]\n", filename.c_str() );
		return -1;
	}

	return 0;
}
