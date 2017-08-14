using System;
using System.Threading.Tasks;
using System.Web.Http;

namespace LoginServer.Request
{
	public class LoginController : ApiController
	{
		[Route("Request/Login")]
		[HttpPost]
		public async Task<LoginRes> LoginRequest(LoginReq reqPacket)
		{
			var resPacket = new LoginRes();

			// MongoDB에서 로그인을 요청한 유저의 정보를 찾아본다.
			var userValidation = await DB.MongoDbManager.GetUserVaildtion(reqPacket.UserId, reqPacket.UserPw);

			if (userValidation.Result == ErrorCode.None)
			{
				// 토큰을 생성하여 기록한다.
				resPacket.Token = TokenGenerator.GetInstance().CreateToken();

				// 토큰 값을 레디스에 기록한다.
				await DB.AuthTokenManager.RegistAuthToken(reqPacket.UserId, resPacket.Token);
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
			var debugString = signInPacket.UserId + "/" + signInPacket.UserPw + " send Sign In Request";
			Console.WriteLine(debugString);

			var resPacket = new LoginRes();

			var userValidation = await DB.MongoDbManager.JoinUserValidation(signInPacket.UserId, signInPacket.UserPw);

			if (userValidation.Result == ErrorCode.None)
			{
				resPacket.Result = (short)ErrorCode.None;
				// 토큰을 생성하여 기록한다.
				resPacket.Token = TokenGenerator.GetInstance().CreateToken();

				// 토큰값을 레디스에 기록한다.
				await DB.AuthTokenManager.RegistAuthToken(signInPacket.UserId, resPacket.Token);
			}
			else
			{
				resPacket.Result = (short)userValidation.Result;
			}

			return resPacket;
		}
	}
}
