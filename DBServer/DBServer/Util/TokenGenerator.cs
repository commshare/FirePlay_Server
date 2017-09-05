using System;
using IdGen;

namespace DBServer.Util
{
    class TokenGenerator
    {
        // 클래스 싱글톤 구현.
        private static TokenGenerator _instance = null;

        public static TokenGenerator GetInstance()
        {
            return _instance ?? (_instance = new TokenGenerator());
        }

        // 클래스 생성자.
        protected TokenGenerator()
        {
            // 토큰 생성일 설정.
            var tokenStartDate = new DateTime(2017, 8, 14, 0, 0, 0, DateTimeKind.Local);

            /*
			 * 토큰 생성 비트 설정
			 * 전체 32비트, TokenGenerator-id에 2비트, 16에 시퀀스 비트
			 */
            var tokenCreationBitConfig = new MaskConfig(32, 2, 16);

            _generator = new IdGenerator(0, tokenStartDate, tokenCreationBitConfig);
        }

        // 토큰 뽑아내기
        public int CreateToken()
        {
            return (int)_generator.CreateId();
        }

        // 토큰 생성기.
        private IdGenerator _generator;
    }
}
