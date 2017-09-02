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
