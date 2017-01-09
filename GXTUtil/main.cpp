#include "IVGXT.h"

int main(int argc, char **argv)
{
	IVGXT inst;

	if (argc == 3)
	{
		inst.process(argv[1], argv[2]);
	}
	else if (argc == 2)
	{
		inst.gxt2txt(argv[1]);
	}
	else
	{
		inst.txt2gxt("GTA4TEST.txt", "chinese.gxt");
	}

	inst.generate_collection("CHINESECHARACTERS.txt");
	inst.generate_table("table.dat");

	return 0;
}
