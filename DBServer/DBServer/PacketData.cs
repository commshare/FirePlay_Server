using MessagePack;

namespace DBServer
{
    // DB 서버는 게임서버(C++)과의 통신을 위해 MsgPack을 기본적으로 사용.
    // DB 서버와 통신을 하고 싶다면 MsgPack을 사용하여야 함.
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
