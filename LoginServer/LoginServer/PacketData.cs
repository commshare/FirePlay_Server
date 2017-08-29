
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

// DB 서버와 통신하기 위한 패킷 구조체들.
namespace DBServer
{
    public struct UserValidationReq
    {
        public string UserId { get; set; }
        public string EncryptedUserPw { get; set; }
    }

    public struct UserValidationRes
    {
        public short Result { get; set; }
    }

    public struct UserSignInReq
    {
        public string UserId { get; set; }
        public string EncryptedUserPw { get; set; }
    }

    public struct UserSignInRes
    {
        public short Result { get; set; }
        public long Token { get; set; }
    }

    public struct TokenValidationReq
    {
        public string UserId { get; set; }
        public long Token { get; set; }
    }

    public struct TokenValidationRes
    {
        public short Result { get; set; }
    }

    public struct TokenAuthReq
    {
        public string UserId { get; set; }
        public long Token { get; set; }
    }

    public struct TokenAuthRes
    {
        public short Result { get; set; }
    }

    public struct TokenDeleteReq
    {
        public string UserId { get; set; }
        public long Token { get; set; }
    }

    public struct TokenDeleteRes
    {
        public short Result { get; set; }
    }
}
