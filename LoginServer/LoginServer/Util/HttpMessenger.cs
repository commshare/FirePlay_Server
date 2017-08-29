using Newtonsoft.Json;
using System;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Text;
using System.Threading.Tasks;
using MessagePack;

namespace LoginServer.Util
{
    class HttpMessenger
    {
        private const string DBServerAddr = "http://localhost:20000/";

        // Http 형식의 api를 호출해주는 함수.
        // DB 서버와의 통신을 위해 사용.
        public static async Task<Result_t> RequestHttp<Request_t, Result_t>(string address, string reqApi, Request_t reqPacket) where Result_t : new()
        {
            var resultData = new Result_t();

            var api = "http://" + address + reqApi;
            var requestJson = JsonConvert.SerializeObject(reqPacket);

            var content = new ByteArrayContent(Encoding.UTF8.GetBytes(requestJson));
            content.Headers.ContentType = new MediaTypeHeaderValue("application/json");

            var network = new HttpClient();
            var response = await network.PostAsync(api, content).ConfigureAwait(false);

            if (response.IsSuccessStatusCode == false)
            {
                return resultData;
            }

            var responseString = await response.Content.ReadAsStringAsync();
            var responseJson = JsonConvert.DeserializeObject<Result_t>(responseString);

            return responseJson;
        }
    }
}
