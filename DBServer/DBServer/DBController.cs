using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Web.Http;
using DBServer.MongoDB;

namespace DBServer
{
    class DBController : ApiController
    {
        // 유저 정보가 등록되어 있는 것과 일치 하는지를 확인해주는 함수.
        [Route("DB/UserValidation")]
        [HttpPost]
        public static async Task<UserValidationRes> GetUserVaildation(UserValidationReq req)
        {

            
        }

    }

	public struct UserVaildation
	{
		public string Id;
		public ErrorCode Result;
	}
}
