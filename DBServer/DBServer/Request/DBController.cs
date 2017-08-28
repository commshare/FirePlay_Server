using System;
using System.Threading.Tasks;
using System.Web.Http;

namespace DBServer
{
    public class DBController : ApiController
    {
        // 해당 유저가 가입되어 있는지 확인하는 메소드.
        [Route("DB/UserValidation")]
        [HttpPost]
        public async Task<UserValidationRes> GetUserValidation(UserValidationReq req)
        {
            var res = new UserValidationRes();

            var isUserExisted = await MongoDB.MongoDBManager.IsUserExist(req.UserId, req.EncryptedUserPw);

            res.Result = (short)isUserExisted;

            // TODO :: 여기서 메시지 팩으로 Pack해서 보내주기.

            return res;
        }

        // 유저를 가입시키는 메소드.
        [Route("DB/AddUser")]
        [HttpPost]
        public async Task<UserSignInRes> AddUser(UserSignInReq req)
        {
            var res = new UserSignInRes();


        }

        // 유저의 토큰 값이 일치하는지 확인해주는 메소드.

        // 유저의 토큰 값을 기록하는 메소드.
    }
}
