﻿
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
			DbUser reqDbUser = new DbUser();
			reqDbUser.Id = reqPacket.UserId;
			reqDbUser.Pw = reqPacket.UserPw;

			var userValidation = await MongoDbManager.GetUserVaildtion(reqDbUser);

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
		public async Task<LoginRes> SignInRequest(LoginReq signPacket)
		{
			String debugString = signPacket.UserId.ToString() + "/" + signPacket.UserPw.ToString() + " send Sign In Request";
			Console.WriteLine(debugString);

			var resPacket = new LoginRes();

			DbUser joinDbUser = new DbUser();
			joinDbUser.Id = signPacket.UserId;
			joinDbUser.Pw = signPacket.UserPw;

			var userValidation = await MongoDbManager.JoinUserValidation(joinDbUser);

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