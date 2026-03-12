using UnityEngine;

public class ECGBeep : MonoBehaviour
{
    public float bpm = 72f;
    public AudioSource beep;

    float timer;
    float interval;

    void Start()
    {
        interval = 60f / bpm;
    }

    void Update()
    {
        timer += Time.deltaTime;
        if (timer >= interval)
        {
            timer = 0f;
            beep.Play();
        }
    }
}