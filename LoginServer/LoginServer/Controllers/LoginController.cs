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

			var userValidation = await MongoDbManager.GetUserVaildtion(reqDbUser);



			return resPacket;
		}

		[Route("Request/SignIn")]
		public async Task<LoginRes> SignInRequest(LoginReq signPacket)
		{
			var resPacket = new LoginRes();
			
			DbUser joinDbUser = new DbUser();
			joinDbUser.Id = signPacket.UserId;
			joinDbUser.Pw = signPacket.UserPw;

			var userValidation = await MongoDbManager.JoinUser(joinDbUser);

			if (userValidation.Result == ErrorCode.None)
			{
				resPacket.Result = (short)ErrorCode.None;
				// 토큰을 생성하여 기록한다.
				// 토큰값을 레디스에 기록한다.
			}
			else
			{
				resPacket.Result = (short) userValidation.Result;
			}

			return resPacket;
		}
	}
}
