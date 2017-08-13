

namespace LoginServer
{
	public enum ErrorCode
	{
		None = 0,
		
		ReqLoginIdAlreadyExist = 10,
		ReqLoginInvalidId = 11,
		ReqLoginInvalidPw = 12,
	}
}