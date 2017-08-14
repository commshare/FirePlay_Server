

namespace LoginServer
{
	public struct LoginReq
	{
		public string UserId;
		public string UserPw;
	}

	public struct LoginRes
	{
		public short Result;
		public long Token;
	}

	public struct LogoutReq
	{
		public string UserId;
		public long Token;
	}

	public struct LogoutRes
	{
		public short Result;
	}
}