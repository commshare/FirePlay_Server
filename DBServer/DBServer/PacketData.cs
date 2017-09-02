
namespace DBServer
{
    public struct UserValidationReq
    {
        public string UserId;
        public string EncryptedUserPw;
    }

    public struct UserValidationRes
    {
        public short Result;
    }
}