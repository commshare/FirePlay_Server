using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class LoginManager : MonoBehaviour
{
	public InputField _idField;
	public InputField _pwField;
	public string _id;
	public string _pw;

	public PlayerInfo _info = null;

	private void OnGUI()
	{
		ButtonCheck();
	}

	private void Start()
	{
		SetBackground();	
		MakeInputFields();
	}

	private void SetBackground()
	{
		var prefab = Resources.Load("Prefabs/Background") as GameObject;
		var backgroundInstance = Instantiate(prefab).GetComponent<Background>();

		backgroundInstance.Init();
	}

	private void ButtonCheck()
	{
		if (GUI.Button(new Rect((Screen.width / 2) - 155, Screen.height * 2 / 3, 150, 50), "Login"))
		{
			TryLogin(_id, _pw);
		}

		if (GUI.Button(new Rect((Screen.width / 2) + 5, Screen.height * 2 / 3, 150, 50), "Exit"))
		{
			Application.Quit();
		}
	}

	private void MakeInputFields()
	{
		// 초기값 세팅.
		_idField.text = "ID";
		_pwField.text = "Password";

		var idEvent = new InputField.SubmitEvent();
		idEvent.AddListener(GetId);
		_idField.onEndEdit = idEvent;

		var pwEvent = new InputField.SubmitEvent();
		pwEvent.AddListener(GetPw);
		_pwField.onEndEdit = pwEvent;
	}

	private void GetId(string arg0)
	{
		_id = arg0;
	}

	private void GetPw(string arg0)
	{
		_pw = arg0;
	}

	void TryLogin(string id, string pw)
	{
		// Id, Pw 둘 중 하나라도 비어 있으면 안된다.
		if (string.IsNullOrEmpty(id) || string.IsNullOrEmpty(pw))
		{
			return;
		}

		// TODO :: 서버가 연결안되어있을 경우 다음씬으로 넘어가기 위한 임시 코드
		var infoPrefab = Resources.Load("Prefabs/PlayerInfo") as GameObject;
		if (infoPrefab != null)
		{
			_info = Instantiate(infoPrefab).GetComponent<PlayerInfo>();
			_info.InfoSetting(_id, 0L);

			SceneManager.LoadScene("CharacterSelect");
		}
		else
		{
			Debug.LogAssertion("PlayerInfo Prefab Instantiate Failed");
		}

		try
		{
			var request = new HttpPack.LoginReq()
			{
				UserId = id,
				UserPw = pw
			};

			// Json으로 변환.
			string requestJson = JsonUtility.ToJson(request);

			// Header 생성.
			Dictionary<string, string> headers = new Dictionary<string, string>();
			headers.Add("Content-Type", "application/json");

			byte[] pData = System.Text.Encoding.UTF8.GetBytes(requestJson.ToCharArray());
			var api = new WWW(HttpPack._serverAddr, pData, headers);

			StartCoroutine(WaitForResponse(api));
		}
		catch (UnityException e)
		{
			Debug.Log(e.Message);
		}
	}

	IEnumerator WaitForResponse(WWW www)
	{
		yield return www;

		var response = new HttpPack.LoginRes();
		if (string.IsNullOrEmpty(www.error))
		{
			var content = www.bytes;
			response = JsonUtility.FromJson<HttpPack.LoginRes>(content.ToString());
			Debug.Log("Login Success");

            // TODO : response의 결과값 확인.
            // 결과값에 따라 메시지창 띄워주기.

            if (_info == null)
            {
                var infoPrefab = Resources.Load("Prefab/PlayerInfo") as GameObject;
                var _info = Instantiate(infoPrefab).GetComponent<PlayerInfo>();
            }
            _info.InfoSetting(_id, response.Token);
		}
		else
		{
			Debug.LogAssertion(www.error);
		}
	}
}

public class HttpPack
{
	public static string _serverAddr = "http://127.0.0.1:19000/";

	[System.Serializable]
	public class LoginReq
	{
		public string UserId;
		public string UserPw;
	}

	[System.Serializable]
	public class LoginRes
	{
		public short Result;
		public long Token;
	}

	[System.Serializable]
	public class LogoutReq
	{
		public string UserId;
		public long Token;
	}

	[System.Serializable]
	public class LogoutRes
	{
		public short Result;
	}
}