using System;
using System.Threading.Tasks;
using System.Web.Http;
using MessagePack;
using System.Text;

namespace DBServer
{
    public class DBController : ApiController
    {
        // 해당 유저가 등록되어 있는지 확인하는 메소드.
        [Route("DB/UserValidation")]
        [HttpPost]
        public async Task<UserValidationRes> GetUserValidation(UserValidationReq req)
        {
            var res = new UserValidationRes();

            Console.WriteLine($"UserValidation Request. Id : {req.UserId}, Pw : {req.EncryptedUserPw}");

            var isUserExisted = await DB.MongoDBManager.IsUserExist(req.UserId, req.EncryptedUserPw);

            res.Result = (short)isUserExisted;

            return res;
        }

        // 유저를 등록하는 메소드.
        [Route("DB/AddUser")]
        [HttpPost]
        public async Task<UserSignInRes> AddUser(UserSignInReq req)
        {
            var res = new UserSignInRes();

            var result = await DB.MongoDBManager.AddUser(req.UserId, req.EncryptedUserPw);

            res.Result = (short)result;

            return res;
        }

        // 유저의 토큰 값이 일치하는지 확인해주는 메소드.
        [Route("DB/TokenValidation")]
        [HttpPost]
        public async Task<TokenValidationRes> GetTokenValidation(TokenValidationReq req)
        {
            var res = new TokenValidationRes();

            var result = await DB.AuthTokenManager.CheckAuthToken(req.UserId, req.Token);

            res.Result = (short)result;

            return res;
        }

        // 유저의 토큰 값을 기록하는 메소드.
        [Route("DB/RegistToken")]
        [HttpPost]
        public async Task<TokenAuthRes> GetTokenAuth(TokenAuthReq req)
        {
            var res = new TokenAuthRes();

            try
            {
                await DB.AuthTokenManager.RegistAuthToken(req.UserId, req.Token);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
                res.Result = (short)ErrorCode.RedisTokenRegistError;
                return res;
            }

            res.Result = (short)ErrorCode.None;
            return res;
        }

        // 유저의 토큰 값을 삭제하는 메소드.
        [Route("DB/DeleteToken")]
        [HttpPost]
        public async Task<TokenDeleteRes> DeleteToken(TokenDeleteReq req)
        {
            var res = new TokenDeleteRes();

            // 유효한 값인지 우선 검사.
            var validation = await DB.AuthTokenManager.CheckAuthToken(req.UserId, req.Token);
            if (validation != (short)ErrorCode.None)
            {
                res.Result = (short)validation;
                return res;
            }

            // 유효한 요청이라면 삭제.
            try
            {
                await DB.AuthTokenManager.DeleteAuthToken(req.UserId);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
                res.Result = (short)ErrorCode.RedisTokenDeleteError;
                return res;
            }

            res.Result = (short)ErrorCode.None;
            return res;
        }

    }
}
