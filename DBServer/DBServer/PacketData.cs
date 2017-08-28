using MessagePack;

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
