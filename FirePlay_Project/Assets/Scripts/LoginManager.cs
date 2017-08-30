using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;
using UnityEngine.Networking;
using System;
using System.Text;

public class LoginManager : MonoBehaviour
{
	public InputField _idField;
	public InputField _pwField;
    public Config _config;
	public string _id;
	public string _pw;

	public PlayerInfo _info = null;

	private void OnGUI()
	{
		ButtonCheck();
	}

	private void Start()
	{
        LoadConfig();
		SetBackground();	
		MakeInputFields();
	}
    
    private void LoadConfig()
    {
        var configText = Resources.Load<TextAsset>("Data/ServerConfig").text;
        _config = Config.CreateFromText(configText);
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

        try
        {
            var request = new HttpPack.LoginReq()
            {
                UserId = id,
                UserPw = pw
            };
            var jsonStr = JsonUtility.ToJson(request);

            string loginRequestUrl = _config.GetHttpString() + "Request/Login";
            StartCoroutine(PostRequest(loginRequestUrl, jsonStr));

        }
        catch (UnityException e)
        {
            Debug.Log(e.Message);
        }
	}

    IEnumerator WaitForResponse(string id, string pw)
    {
        string loginRequest = _config.GetHttpString() + "Request/Login"; 
        Debug.LogFormat("Waiting For Response... To {0}", loginRequest);

        List<IMultipartFormSection> formData = new List<IMultipartFormSection>();
        formData.Add(new MultipartFormDataSection("UserId", id));
        formData.Add(new MultipartFormDataSection("UserPw", pw));

        var www = UnityWebRequest.Post(loginRequest, formData);
        yield return www.Send();

        var response = new HttpPack.LoginRes();
        if (!www.isError)
        {
            Debug.Log("Login Success");
        }
        else
        {
            Debug.LogAssertion(www.error);
        }
    }

    IEnumerator PostRequest(string url, string bodyJsonString)
    {
        var request = new UnityWebRequest(url, "POST");
        byte[] bodyRaw = new System.Text.UTF8Encoding().GetBytes(bodyJsonString);
        request.uploadHandler = (UploadHandler)new UploadHandlerRaw(bodyRaw);
        request.downloadHandler = (DownloadHandler)new DownloadHandlerBuffer();
        request.SetRequestHeader("Content-Type", "application/json");

        yield return request.Send();

        Debug.Log("Response: " + request.downloadHandler.text);
    }
}

public struct Config
{
    [SerializeField]
    public string LoginServerAddr;
    [SerializeField]
    public string Port;

    public static Config CreateFromText(string text)
    {
        Config instance;
        try
        {
            instance = JsonUtility.FromJson<Config>(text);
        }
        catch (Exception e)
        {
            Debug.LogErrorFormat("[Config] Cannot parse Config from source - {0}", text);
            throw;
        }

        return instance;
    }

    public string GetHttpString()
    {
        var connectString = "http://" + LoginServerAddr + ":" + Port + "/";
        return connectString;
    }
}

public class HttpPack
{

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