using System;
using System.Security.Cryptography;

namespace LoginServer.Util
{
	class Encrypter
	{
		// 출처 : http://codingcoding.tistory.com/139
		public static string EncryptString(string message)
		{
			byte[] results;
			var utf8 = new System.Text.UTF8Encoding();

			// MD5 해쉬 생성기 초기화..
			var hashProvider = new MD5CryptoServiceProvider();
			var tdesKey = hashProvider.ComputeHash(utf8.GetBytes(Passphrase));

			// 암호화 알고리즘 오브젝트 생성.
			var tdesAlgorithm = new TripleDESCryptoServiceProvider();

			// 암호화 알고리즘 세팅.
			tdesAlgorithm.Key = tdesKey;
			tdesAlgorithm.Mode = CipherMode.ECB;
			tdesAlgorithm.Padding = PaddingMode.PKCS7;

			var dataToEncrypt = utf8.GetBytes(message);

			// 문자열 암호화 작업.
			try
			{
				var encryptor = tdesAlgorithm.CreateEncryptor();
				results = encryptor.TransformFinalBlock(dataToEncrypt, 0, dataToEncrypt.Length);
			}
			catch (Exception e)
			{
				Console.WriteLine(e.Message);
				throw;
			}
			finally
			{
				tdesAlgorithm.Clear();
				hashProvider.Clear();
			}

			// base64로 변환하여 리턴.
			return Convert.ToBase64String(results);
		}

		// TODO :: key값을 파일로 읽어들이도록.
		private const string Passphrase = "Test Phrase";
	}
}
