using Newtonsoft.Json;
using System;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Text;
using System.Threading.Tasks;
using MessagePack;
using System.Net;

namespace LoginServer.Util
{
    class HttpMessenger
    {
        // Http 형식의 api를 호출해주는 함수.
        // DB 서버와의 통신을 위해 사용.
        public static async Task<Result_t> RequestHttp<Request_t, Result_t>(string address, string reqApi, Request_t reqPacket) where Result_t : new()
        {
            var resultData = new Result_t();

            var api = address + reqApi;
            var requestJson = JsonConvert.SerializeObject(reqPacket);

            var content = new ByteArrayContent(Encoding.UTF8.GetBytes(requestJson));
            content.Headers.ContentType = new MediaTypeHeaderValue("application/json");

            var network = new HttpClient();
            HttpResponseMessage response = null;
            string responseString = "";

            try
            {
                response = await network.PostAsync(api, content).ConfigureAwait(false);

                if (response.IsSuccessStatusCode == false)
                {
                    return resultData;
                }

                responseString = await response.Content.ReadAsStringAsync();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
                if (response == null)
                {
                    response = new HttpResponseMessage();
                }
                 response.StatusCode = HttpStatusCode.InternalServerError;
                 response.ReasonPhrase = string.Format("RestHttpClient.SendRequest failed: {0}", e);
            }

            var responseJson = JsonConvert.DeserializeObject<Result_t>(responseString);

            return responseJson;
        }
    }
}
