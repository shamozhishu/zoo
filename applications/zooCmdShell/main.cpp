#include <thread>
#include <stdio.h>
#include <direct.h>
#include <zooCmdLoader/ZooCmdLoader.h>

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

static char* strtolower(char* str)
{
	if (!str)
		return str;
	char* ret = str;
	while (*str != '\0')
	{
		if ((*str >= 'A') && (*str <= 'Z'))
		{
			*str = *str + 32;
			str++;
		}
		else
			str++;
	}
	return ret;
}

static void parsecmdline()
{
	char szCmdline[128];
	while (true)
	{
		printf("--zooCmd: >> ");
		fgets(szCmdline, sizeof(szCmdline), stdin);
		if (feof(stdin))
			break;

		char* pszCmdline = strtrim(szCmdline);
		if (0 == _stricmp(pszCmdline, "cls"))
		{
			system("cls");
			continue;
		}

		if (zooCmd_Send(pszCmdline))
		{
			if (0 == _strnicmp(pszCmdline, "exit", 4))
				break;

			bool bRet;
			int iRet;
			float fRet;
			double dRet;

			if (nullptr != strstr(strtolower(pszCmdline), "bool") && zooCmd_BoolValue("b", &bRet))
			{
				char szbuf[32];
				if (bRet)
					sprintf_s(szbuf, sizeof(szbuf), "true");
				else
					sprintf_s(szbuf, sizeof(szbuf), "false");
				printf("bool return value: %s\n", szbuf);
			}
			else if (nullptr != strstr(strtolower(pszCmdline), "int") && zooCmd_IntValue("i", &iRet))
				printf("int return value: %d\n", iRet);
			else if (nullptr != strstr(strtolower(pszCmdline), "float") && zooCmd_FloatValue("f", &fRet))
				printf("float return value: %.3f\n", fRet);
			else if (nullptr != strstr(strtolower(pszCmdline), "double") && zooCmd_DoubleValue("d", &dRet))
				printf("double return value: %.6f\n", dRet);
			else if (nullptr != strstr(strtolower(pszCmdline), "str"))
			{
				const char* szRet = zooCmd_StringValue("str");
				if (szRet && strcmp(szRet, "") != 0)
					printf("string return value: %s\n", szRet);
			}
			else if (nullptr != strstr(strtolower(pszCmdline), "err"))
			{
				const char* szTips = zooCmd_TipMessage();
				if (szTips && strcmp(szTips, "") != 0)
					printf("tip message: %s\n", szTips);
			}
		}
	}
}

int main(int argc, char* args[])
{
	const char* datadir = nullptr;
	if (argc > 1)
		datadir = args[1];

	int cmdcount = argc - 2;
	char** cmdset = nullptr;
	if (cmdcount > 0)
	{
		cmdset = new char*[cmdcount];
		for (int i = 0; i < cmdcount; ++i)
			cmdset[i] = args[i + 2];
	}

	zooCmdL_Load();
	if (!zooCmd_InitA("zooCmd_osg", datadir ? datadir : _getcwd(nullptr, 0)))
	{
		if (cmdset)
			delete[] cmdset;
		return -1;
	}

	zooCmd_Setup(0, 0, 1, 0);

	for (int i = 0; i < cmdcount; ++i)
		zooCmd_Register(cmdset[i]);

	if (cmdset)
		delete[] cmdset;

	std::thread cmdline(parsecmdline);
	zooCmd_Run();
	cmdline.join();
	zooCmd_Destroy();
	return 0;
}
