#include <QtCore/QtCore>
#include <iostream>
#include <clocale>
#include <locale>
#include "Util/Exception.h"
#include "Util/IniSettings.h"

#include "GreisMessage.h"
#include "GreisMessageStream.h"
#include "JpsFile.h"
#include "Util/FileException.h"
#include "DatabaseWriter.h"

#include <fstream>
#include <algorithm>

using namespace std;
using namespace Greis;

#include "Util/Logger.h"
#include "Util/Path.h"
#include "Util/BitConverter.h"

using namespace Util;

int main(int argc, char *argv[])
{
    try
    {
        std::setlocale(LC_ALL, "Russian_Russia.1251");
        std::locale::global(std::locale("Russian_Russia.1251"));

        QCoreApplication a(argc, argv);

        // ������
        QTextCodec* codec = QTextCodec::codecForName("UTF-8");
        QTextCodec* codecMs = QTextCodec::codecForName("Windows-1251");
        QTextCodec::setCodecForCStrings(codecMs);
        QTextCodec::setCodecForLocale(codec);
        QTextCodec::setCodecForTr(codec);

        // static init
        sLogger.Initialize(Path::Combine(Path::ApplicationDirPath(), "logger.config.xml"));
        StdMessage_t::parseXml();
        sIniSettings.Initialize(Path::Combine(Path::ApplicationDirPath(), "config.ini"));
        QStringList args = a.arguments();

        if (args.count() < 2)
        {
            sLogger.Error(QString("Invalid command line arguments. Usage: JpsParser <input Jps file>"));
            return 1;
        }
        QString filename = args[1];

#ifdef _DEBUG
        {
            string log1;
            StdMessageStream stream(filename);
            Message_t::Pointer_t msg;
            while((msg = stream.Next(true)).get())
            {
                log1.append(boost::shared_dynamic_cast<StdMessage_t>(msg)->id() + " ");
            }
            std::ofstream fileLog("jpsTrace.txt");
            fileLog.write(log1.c_str(), log1.size());
            fileLog.close();
        }
#endif

        // �������� � �������
        JpsFile_t::Pointer_t jpsFile(new JpsFile_t(filename));
        // ������ � ����
        /*Connection connection = Connection::FromSettings("Db");
        DatabaseWriter writer;
        writer.Connect(connection, "source1");
        writer.AddData(jpsFile);*/

#ifdef _DEBUG
        std::ofstream jps2("jps2.jps");
        jpsFile->toBinaryStream(jps2);
        jps2.close();
#endif

        //sLog.addInfo(QString("��������� ������� ���������."));
        system("PAUSE");
        return 0;
    }
    catch (Exception& e)
    {
        sLogger.Error(e.what());
        return 1;
    }
}
