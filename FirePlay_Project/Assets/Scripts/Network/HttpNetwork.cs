using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Networking;

public class HttpNetwork : MonoBehaviour
{
    public IEnumerator PostRequest<T>(string url, string bodyJsonString, Func<T, bool> onSuccess)
    {
        var request = new UnityWebRequest(url, "POST");
        byte[] bodyRaw = new System.Text.UTF8Encoding().GetBytes(bodyJsonString);

        request.uploadHandler = (UploadHandler)new UploadHandlerRaw(bodyRaw);
        request.downloadHandler = (DownloadHandler)new DownloadHandlerBuffer();
        request.SetRequestHeader("Content-Type", "application/json");

        yield return request.Send();

        if (request.isError)
        {
            Debug.LogError("Http Post Failed");
        }
        else
        {
            Debug.Log("Response : " + request.downloadHandler.text);

            if (request.responseCode == 200)
            {
                Debug.Log("Request finished successfully");
                // 받은 정보 처리하도록 넘겨준다..
                onSuccess(JsonUtility.FromJson<T>(request.downloadHandler.text));
            }
            else if (request.responseCode == 401)
            {
                Debug.Log("Error 401 : Unauthorized. Resubmitted Request");
                StartCoroutine(PostRequest<T>(url, bodyJsonString, onSuccess));
            }
            else
            {
                Debug.Log("Request failed (status : " + request.responseCode + ")");
            }
        }
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