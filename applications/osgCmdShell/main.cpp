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
		printf("--cmd: >> ");
		fgets(szCmdline, sizeof(szCmdline), stdin);
		if (feof(stdin))
			break;

		const char* pszCmdline = strtrim(szCmdline);
		osgCmd_Send(pszCmdline);
		if (0 == _stricmp(pszCmdline, "exit"))
			break;
	}
}

int main(int argc, char* args[])
{
	const char* workdir = nullptr;
	if (argc > 1)
		workdir = args[1];

	osgCmd_Init(workdir);

	int i = 2;
	while (i < argc)
		osgCmd_Register(args[i++]);

	std::thread cmdline(parsecmdline);
	osgCmd_Run();
	cmdline.join();
	osgCmd_Destroy();

	return 0;
}
