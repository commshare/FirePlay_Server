
using System;
using System.Threading.Tasks;
using System.Web.Http;

namespace LoginServer
{
	public class LoginController : ApiController
	{
		[Route("Request/Login")]
		[HttpPost]
		public async Task<LoginRes> LoginRequest(LoginReq reqPacket)
		{
			var resPacket = new LoginRes();

			// MongoDB에서 로그인을 요청한 유저의 정보를 찾아본다.
			var userValidation = await MongoDbManager.GetUserVaildtion(reqPacket.UserId, reqPacket.UserPw);

			if (userValidation.Result == ErrorCode.None)
			{
				// 토큰을 생성하여 기록한다.
				// 토큰 값을 레디스에 기록한다.
			}
			else
			{
				resPacket.Result = (short) userValidation.Result;
			}

			return resPacket;
		}

		[Route("Request/SignIn")]
		[HttpPost]
		public async Task<LoginRes> SignInRequest(LoginReq signInPacket)
		{
			String debugString = signInPacket.UserId.ToString() + "/" + signInPacket.UserPw.ToString() + " send Sign In Request";
			Console.WriteLine(debugString);

			var resPacket = new LoginRes();

			var userValidation = await MongoDbManager.JoinUserValidation(signInPacket.UserId, signInPacket.UserPw);

			if (userValidation.Result == ErrorCode.None)
			{
				resPacket.Result = (short)ErrorCode.None;
				// 토큰을 생성하여 기록한다.
				// 토큰값을 레디스에 기록한다.
			}
			else
			{
				resPacket.Result = (short)userValidation.Result;
			}

			return resPacket;
		}
	}
}
