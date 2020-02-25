#include <osgCmd.h>
#include <stdio.h>
#include <thread>

static char* strtrim(char* s)
{
	char* p = s;
	char* q = s;
	while (*p == ' ' || *p == '\t' || *p == '\n') ++p;
	while (*q++ = *p++);
	q -= 2;
	while (*q == ' ' || *q == '\t' || *q == '\n') --q;
	*(q + 1) = '\0';
	return s;
}

static void parsecmdline()
{
	char szCmdline[128];
	while (true)
	{
		printf("--osgCmd: >> ");
		fgets(szCmdline, sizeof(szCmdline), stdin);
		if (feof(stdin))
			break;

		const char* pszCmdline = strtrim(szCmdline);
		if (0 == _stricmp(pszCmdline, "cls"))
		{
			system("cls");
			continue;
		}

		if (osgCmd_Send(pszCmdline) && 0 == strcmp(pszCmdline, "--exit"))
			break;
	}
}

int main(int argc, char* args[])
{
	const char* workdir = nullptr;
	if (argc > 1)
		workdir = args[1];

	int cmdcount = argc - 2;
	char** cmdset = nullptr;
	if (cmdcount > 0)
	{
		cmdset = new char*[cmdcount];
		for (int i = 0; i < cmdcount; ++i)
			cmdset[i] = args[i + 2];
	}
	
	osgCmd_Init(cmdcount, cmdset, workdir);

	if (cmdset)
		delete[] cmdset;

	std::thread cmdline(parsecmdline);
	osgCmd_Run();
	cmdline.join();
	osgCmd_Destroy();

	return 0;
}
