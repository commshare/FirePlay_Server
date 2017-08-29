using MessagePack;

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

namespace DBServer
{
    [MessagePackObject]
    public struct UserValidationReq
    {
        [Key(0)]
        public string UserId { get; set; }
        [Key(1)]
        public string EncryptedUserPw { get; set; }
    }

    [MessagePackObject]
    public struct UserValidationRes
    {
        [Key(0)]
        public short Result { get; set; }
    }

    [MessagePackObject]
    public struct UserSignInReq
    {
        [Key(0)]
        public string UserId { get; set; }
        [Key(1)]
        public string EncryptedUserPw { get; set; }
    }

    [MessagePackObject]
    public struct UserSignInRes
    {
        [Key(0)]
        public short Result { get; set; }
        [Key(1)]
        public long Token { get; set; }
    }

    [MessagePackObject]
    public struct TokenValidationReq
    {
        [Key(0)]
        public string UserId { get; set; }
        [Key(1)]
        public long Token { get; set; }
    }

    [MessagePackObject]
    public struct TokenValidationRes
    {
        [Key(0)]
        public short Result { get; set; }
    }

    [MessagePackObject]
    public struct TokenAuthReq
    {
        [Key(0)]
        public string UserId { get; set; }
        [Key(1)]
        public long Token { get; set; }
    }

    [MessagePackObject]
    public struct TokenAuthRes
    {
        [Key(0)]
        public short Result { get; set; }
    }
}