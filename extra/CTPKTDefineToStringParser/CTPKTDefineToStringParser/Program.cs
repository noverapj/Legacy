using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Globalization;

namespace CTPKTDefineToStringParser
{
    class Program
    {
        static void Main(string[] args)
        {
            List<string> writeLine = new List<string>();

            writeLine.Add("#ifndef ___PROTOCOL_HELPER_H__");
            writeLine.Add("#define ___PROTOCOL_HELPER_H__");
            writeLine.Add("\r\n");
            writeLine.Add("#include \"Protocol.h\"");
            writeLine.Add("\r\n");

            MakeClientToServerPacketForProtocolHelper(writeLine, args);
            MakeServerToServerPacketForProtocolHelper(writeLine, args);

            writeLine.Add("\r\n");
            writeLine.Add("#endif");

            System.IO.File.WriteAllLines(args[1], writeLine.ToArray());
        }

        public static void MakeClientToServerPacketForProtocolHelper(List<string> writeLine, string[] args)
        {
            writeLine.Add("#define GetStringOfCTPCKPacket(packetID, p) \\");
            writeLine.Add("     switch( packetID ) \\");
            writeLine.Add("     { \\");

            int counter = 0;
            string line;

            System.IO.StreamReader readFile = new System.IO.StreamReader(args[0]);

            while ((line = readFile.ReadLine()) != null)
            {
                if (!line.Contains("#define CTPK") && !line.Contains("#define CUPK"))
                    continue;

                if (line.Contains("//"))
                    continue;

                if (!line.Contains("0x"))
                    continue;

                string[] strings = line.Split(new string[] { " ", "0x" }, StringSplitOptions.None);

                string packetString = RemoveSpecialCharacters(strings[1]);

                {
                    writeLine.Add("     case " + packetString + ": \\");
                    writeLine.Add("         (p) = " + "\"" + packetString + "\"; \\");
                    writeLine.Add("         break; \\");
                }

                Console.WriteLine(line);
                counter++;
            }

            {
                writeLine.Add("     default: \\");
                writeLine.Add("         char buf[100]; \\");
                writeLine.Add("         sprintf(buf, \"0x%x\", packetID); \\");
                writeLine.Add("         (p) = buf; \\");
                writeLine.Add("         break; \\");
                writeLine.Add("     } \\");
            }

            writeLine.Add("\r\n");
            writeLine.Add("\r\n");

            readFile.Close();
        }

        public static void MakeServerToServerPacketForProtocolHelper(List<string> writeLine, string[] args)
        {
            writeLine.Add("#define GetStringOfSSTPKPacket(packetID, p) \\");
            writeLine.Add("     switch( packetID ) \\");
            writeLine.Add("     { \\");


            int counter = 0;
            string line;

            System.IO.StreamReader readFile = new System.IO.StreamReader(args[0]);
            while ((line = readFile.ReadLine()) != null)
            {
                if (!line.Contains("#define SSTPK"))
                    continue;

                if (line.Contains("//"))
                    continue;

                if (!line.Contains("0x"))
                    continue;

                string[] strings = line.Split(new string[] { " ", "0x" }, StringSplitOptions.None);

                string packetString = RemoveSpecialCharacters(strings[1]);

                {
                    writeLine.Add("     case " + packetString + ": \\");
                    writeLine.Add("         (p) = " + "\"" + packetString + "\"; \\");
                    writeLine.Add("         break; \\");
                }

                Console.WriteLine(line);
                counter++;
            }

            {
                writeLine.Add("     default: \\");
                writeLine.Add("         char buf[100]; \\");
                writeLine.Add("         sprintf(buf, \"0x%x\", packetID); \\");
                writeLine.Add("         (p) = buf; \\");
                writeLine.Add("         break; \\");
                writeLine.Add("     } \\");
            }

            writeLine.Add("\r\n");
            writeLine.Add("\r\n");

            readFile.Close();
        }

        public static string RemoveSpecialCharacters(string str)
        {
            return Regex.Replace(str, "[^a-zA-Z0-9_.]+", "", RegexOptions.Compiled);
        }
    }
}
