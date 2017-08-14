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

			// 유저 패스워드를 암호화 한다.
			var encryptedPassword = Util.Encrypter.EncryptString(reqPacket.UserPw);

			Console.WriteLine($"Id({reqPacket.UserId}), Pw({encryptedPassword}), Send Login Request");

			// MongoDB에서 로그인을 요청한 유저의 정보를 찾아본다.
			var userValidation = await DB.MongoDbManager.GetUserVaildation(reqPacket.UserId, encryptedPassword);

			if (userValidation.Result == ErrorCode.None)
			{
				// 토큰을 생성하여 기록한다.
				resPacket.Token = TokenGenerator.GetInstance().CreateToken();

				// 토큰 값을 레디스에 기록한다.
				await DB.AuthTokenManager.RegistAuthToken(reqPacket.UserId, resPacket.Token);

				Console.WriteLine($"Id({reqPacket.UserId}), Pw({encryptedPassword}), Log In Success");
			}
			else
			{
				resPacket.Result = (short)userValidation.Result;

				Console.WriteLine($"Id({reqPacket.UserId}), Pw({encryptedPassword}), Log In Failed");
			}

			return resPacket;
		}

		[Route("Request/SignIn")]
		[HttpPost]
		public async Task<LoginRes> SignInRequest(LoginReq signInPacket)
		{
			var resPacket = new LoginRes();

			// 유저 패스워드를 암호화 한다.
			var encryptedPassword = Util.Encrypter.EncryptString(signInPacket.UserPw);

			Console.WriteLine($"Id({signInPacket.UserId}), Pw({encryptedPassword}), Send SignIn Request");

			// 회원가입이 가능한 정보인지를 MongoDB에 조회한다.
			var userValidation = await DB.MongoDbManager.JoinUserValidation(signInPacket.UserId, encryptedPassword);

			// 회원가입이 가능하다면
			if (userValidation.Result == ErrorCode.None)
			{
				resPacket.Result = (short)ErrorCode.None;
				// 토큰을 생성하여 기록한다.
				resPacket.Token = TokenGenerator.GetInstance().CreateToken();
				// 토큰값을 레디스에 기록한다.
				await DB.AuthTokenManager.RegistAuthToken(signInPacket.UserId, resPacket.Token);

				Console.WriteLine($"Id({signInPacket.UserId}), Pw({encryptedPassword}), Sign In Success");
			}
			// 불가능하다면 이유를 적고 반환한다.
			else
			{
				resPacket.Result = (short)userValidation.Result;

				Console.WriteLine($"Id({signInPacket.UserId}), Pw({encryptedPassword}), Sign In Failed");
			}

			return resPacket;
		}

		[Route("Request/Logout")]
		[HttpPost]
		public async Task<LogoutRes> LogoutRequest(LogoutReq logoutPacket)
		{
			var resPacket = new LogoutRes();

			Console.WriteLine($"Id({logoutPacket.UserId}), Send Logout Packet");

			// Id가 MongoDB에 없다면 입력값 에러 반환.
			var isIdIsValid = await DB.MongoDbManager.GetIdExist(logoutPacket.UserId);
			if (isIdIsValid == false)
			{
				Console.WriteLine($"Id({logoutPacket.UserId}) is Invalid Id");
				resPacket.Result = (short)ErrorCode.LogoutInvalidId;
				return resPacket;
			}

			// Token이 Redis에 없다면 입력값 에러 반환.
			var isTokenValid = await DB.AuthTokenManager.CheckAuthToken(logoutPacket.UserId, logoutPacket.Token);
			if (isTokenValid == false)
			{
				Console.WriteLine($"Id({logoutPacket.UserId})'s Token({logoutPacket.Token}) is Invalid");
				resPacket.Result = (short) ErrorCode.LogoutInvalidToken;
				return resPacket;
			}

			// 입력 요청값이 Valid하다고 판단되면 Redis에서 값 삭제.
			await DB.AuthTokenManager.DeleteAuthToken(logoutPacket.UserId);
			resPacket.Result = (short) ErrorCode.None;
			Console.WriteLine($"Id({logoutPacket.UserId}), Logout Success");

			return resPacket;
		}
	}
}
