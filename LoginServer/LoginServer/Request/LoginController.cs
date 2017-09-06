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

            try
            {
                // DB Server에 유저가 가입되어 있는지를 조사한다.
                var userValidationReq = new DBServer.UserValidationReq()
                {
                    UserId = reqPacket.UserId,
                    EncryptedUserPw = encryptedPassword
                };

                var userValidationRes = await Util.HttpMessenger.RequestHttp<DBServer.UserValidationReq, DBServer.UserValidationRes>(
                    LoginServerMain.config.DBServerAddress, "DB/UserValidation", userValidationReq);

                // 가입되어 있지 않다면 에러 반환.
                if (userValidationRes.Result != (short)ErrorCode.None)
                {
                    Console.WriteLine($"Id({reqPacket.UserId}), Pw({encryptedPassword}), Invalid LoginRequest : Error({userValidationRes.Result})");
                    resPacket.Result = userValidationRes.Result;
                    resPacket.Token = -1;
                    return resPacket;
                }

                // 가입되어 있다면 토큰을 생성한다.
                resPacket.Token = TokenGenerator.GetInstance().CreateToken();

                // DB Server에 토큰을 등록한다.
                var tokenAuthReq = new DBServer.TokenAuthReq()
                {
                    UserId = reqPacket.UserId,
                    Token = resPacket.Token
                };

                var tokenAuthRes = await Util.HttpMessenger.RequestHttp<DBServer.TokenAuthReq, DBServer.TokenAuthRes>(
                    LoginServerMain.config.DBServerAddress, "DB/RegistToken", tokenAuthReq);

                // 토큰 등록이 실패했다면 에러 반환.
                if (tokenAuthRes.Result != (short)ErrorCode.None)
                {
                    Console.WriteLine($"Id({reqPacket.UserId}), Pw({encryptedPassword}), Token Regist failed : Error({tokenAuthRes.Result})");
                    resPacket.Result = userValidationRes.Result;
                    resPacket.Token = -1;
                    return resPacket;
                }

                // 모든 절차가 완료되었다면 정상값 반환.
                Console.WriteLine($"Id({reqPacket.UserId}), Pw({encryptedPassword}), Login Success");
                resPacket.Result = (short)ErrorCode.None;
                return resPacket;
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
                return resPacket;
            }
        }

        [Route("Request/SignIn")]
        [HttpPost]
        public async Task<LoginRes> SignInRequest(LoginReq signInPacket)
        {
            var resPacket = new LoginRes();

            // 유저 패스워드를 암호화 한다.
            var encryptedPassword = Util.Encrypter.EncryptString(signInPacket.UserPw);

            Console.WriteLine($"Id({signInPacket.UserId}), Pw({encryptedPassword}), Send SignIn Request");

            // 회원가입이 가능한 정보인지를 DB 서버에 조회한다.
            var userValidationReq = new DBServer.UserValidationReq()
            {
                UserId = signInPacket.UserId,
                EncryptedUserPw = encryptedPassword
            };

            var userValidationRes = await Util.HttpMessenger.RequestHttp<DBServer.UserValidationReq, DBServer.UserValidationRes>(
                LoginServerMain.config.DBServerAddress, "DB/UserValidation", userValidationReq);

            // 회원가입이 불가능하다면, 이유를 적고 반환해준다.
            if (userValidationRes.Result != (short)ErrorCode.LoginUserInfoDontExist)
            {
                Console.WriteLine($"Id({signInPacket.UserId}), Pw({encryptedPassword}), Error : ({userValidationRes.Result})");
                resPacket.Result = (short)ErrorCode.SignInInvalidId;
                resPacket.Token = -1;
                return resPacket;
            }

            // 회원가입이 가능하다면 DB 서버에 회원 가입을 요청한다.
            var userSignInReq = new DBServer.UserSignInReq()
            {
                UserId = signInPacket.UserId,
                EncryptedUserPw = encryptedPassword
            };


            var userSignInRes = await Util.HttpMessenger.RequestHttp<DBServer.UserSignInReq, DBServer.UserSignInRes>(
                LoginServerMain.config.DBServerAddress, "DB/AddUser", userSignInReq);

            // 회원가입이 실패했다면, 이유를 적고 반환해준다.
            if (userSignInRes.Result != (short)ErrorCode.None)
            {
                Console.WriteLine($"Id({signInPacket.UserId}), Pw({encryptedPassword}), Error : ({userSignInRes.Result})");
                resPacket.Result = userSignInRes.Result;
                resPacket.Token = -1;
                return resPacket;
            }

            // 토큰을 생성하고 DB 서버에 등록한다.
            resPacket.Token = TokenGenerator.GetInstance().CreateToken();

            var tokenAuthReq = new DBServer.TokenAuthReq()
            {
                UserId = signInPacket.UserId,
                Token = resPacket.Token
            };

            var tokenAuthRes = await Util.HttpMessenger.RequestHttp<DBServer.TokenAuthReq, DBServer.TokenAuthRes>(
                LoginServerMain.config.DBServerAddress, "DB/RegistToken", tokenAuthReq);

            // 토큰 등록이 실패했다면 에러 반환.
            if (tokenAuthRes.Result != (short)ErrorCode.None)
            {
                Console.WriteLine($"Id({signInPacket.UserId}), Pw({encryptedPassword}), Error : ({tokenAuthRes.Result})");
                resPacket.Result = userValidationRes.Result;
                resPacket.Token = -1;
                return resPacket;
            }

            // 모든 절차가 완료되었다면 정상값을 반환한다.
            Console.WriteLine($"Id({signInPacket.UserId}), Pw({encryptedPassword}), Sign In Success");
            resPacket.Result = (short)ErrorCode.None;
            return resPacket;
        }

        [Route("Request/Logout")]
        [HttpPost]
        public async Task<LogoutRes> LogoutRequest(LogoutReq logoutPacket)
        {
            var resPacket = new LogoutRes();

            Console.WriteLine($"Id({logoutPacket.UserId}), Send Logout Packet");

            // DB서버에 유저의 토큰 값을 내려달라고 요청.
            var tokenDeleteReq = new DBServer.TokenDeleteReq()
            {
                UserId = logoutPacket.UserId,
                Token = logoutPacket.Token
            };

            var tokenDeleteRes = await Util.HttpMessenger.RequestHttp<DBServer.TokenDeleteReq, DBServer.TokenDeleteRes>(
                LoginServerMain.config.DBServerAddress, "DB/DeleteToken", tokenDeleteReq);

            if (tokenDeleteRes.Result != (short)ErrorCode.None)
            {
                Console.WriteLine($"Id({logoutPacket.UserId}), Error : {tokenDeleteRes.Result}");
                resPacket.Result = tokenDeleteRes.Result;
                return resPacket;
            }

            Console.WriteLine($"Id({logoutPacket.UserId}), Logout Success");
            resPacket.Result = (short)ErrorCode.None; 
            return resPacket;
        }
    }
}
