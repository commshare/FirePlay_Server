using Newtonsoft.Json;
using System;
using System.IO;

namespace LoginServer
{
    public class ServerConfig
    {
        public string DBServerAddress;
    }

	public static class LoginServerMain
	{
        static public ServerConfig config = null;

        public static void Init()
        {
#if DEBUG
            config = new ServerConfig
            {
                DBServerAddress = "127.0.0.1"
            };
#else
            using (StreamReader r = new StreamReader("../../PrivateData/address.json"))
            {
                string json = r.ReadToEnd();
                config = new ServerConfig();

                config = JsonConvert.DeserializeObject<ServerConfig>(json);
            }
#endif
        }
    }
}
