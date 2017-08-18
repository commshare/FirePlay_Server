using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Background : MonoBehaviour
{
	private Sprite _backgroundSprite = null;
	private SpriteRenderer _renderer = null;

	public void Init()
	{
		_backgroundSprite = Resources.Load("GUI/FirePlayBackground", typeof(Sprite)) as Sprite;
		_renderer = this.GetComponent<SpriteRenderer>();

		_renderer.sprite = _backgroundSprite;
	}
}