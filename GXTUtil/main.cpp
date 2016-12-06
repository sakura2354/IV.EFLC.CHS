#include "IVGXT.h"
#include "print.hpp"

//GXTUtil input output
//GXTUtil IV.EN.gxt IV.CHS.txt
//GXTUtil IV.CHS.txt IV.CHS.gxt

// IV.CHS.gxt
// EFLC.CHS.gxt
// TLAD.CHS.gxt
// TBOGT.CHS.gxt
//

int main(int argc, char **argv)
{
	IVGXT inst;

	if (argc == 3)
	{
		inst.process(argv[1], argv[2]);
	}
	else if (argc == 2)
	{

	}

	return 0;
}
