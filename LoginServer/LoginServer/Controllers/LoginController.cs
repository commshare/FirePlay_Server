using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Threading.Tasks;
using System.Web.Http;
using LoginServer.MongoDB;

namespace LoginServer.Controllers
{
	public class LoginController : ApiController
	{
		[Route("Request/Login")]
		public async Task<LoginRes> LoginRequest(LoginReq reqPacket)
		{
			var resPacket = new LoginRes();

			// MongoDB에서 로그인을 요청한 유저의 정보를 찾아본다.
			DbUser reqDbUser = new DbUser();
			reqDbUser.Id = reqPacket.UserId;
			reqDbUser.Pw = reqPacket.UserPw;

			var userValidation = await MongoDBManager.GetUserVaildtion(reqDbUser);

			return resPacket;
		}

	}
}
