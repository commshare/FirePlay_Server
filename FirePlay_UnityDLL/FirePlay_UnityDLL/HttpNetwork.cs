using System;
using System.Collections.Generic;
using System.Linq;
using Newtonsoft.Json;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Text;
using System.Threading.Tasks;

namespace FirePlay_UnityDLL
{
    public class HttpNetwork
    {
		public class LoginReq
		{
			public string UserId;
			public string UserPw;
		}

		public class LoginRes
		{
			public short Result;
			public long Token;
		}

		public class LogoutReq
		{
			public string UserId;
			public long Token;
		}

		public class LogoutRes
		{
			public short Result;
		}

		public static async Task<Result_t> RequestHttp<Request_t, Result_t>
			(string address, string reqApi, Request_t reqPacket) where Result_t : new()
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
