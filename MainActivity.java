package com.oiwiterio.FlowSheet_NR;

import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.provider.Settings;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;

import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;

public class MainActivity extends AppCompatActivity
{
    static
    {
        System.loadLibrary("FlowSheet_NR");
    }

    @SuppressLint({"SetTextI18n", "QueryPermissionsNeeded"})
    @Override protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        TextView TV1 = findViewById(R.id.TV1);
        Button B1 = findViewById(R.id.B1);
        EditText ET1 = findViewById(R.id.ET1);
        Spinner S1 = findViewById(R.id.S1);

        B1.setOnClickListener(view ->
        {
            LocalDateTime now = LocalDateTime.now();
            DateTimeFormatter formatter = DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss");
            String formattedDate = now.format(formatter);
            if (!ET1.getText().toString().equals(""))
            {
                if (S1.getSelectedItem().toString().equals("图片路径"))
                {
                    if (FileSend("192.168.2.4", "48484", ET1.getText().toString()) == 0)
                        TV1.setText(formattedDate + " 发送成功");
                    else
                        TV1.setText(formattedDate + " 发送失败");
                }
                else if (S1.getSelectedItem().toString().equals("文本"))
                {
                    if (DataSend("192.168.2.4", "58484", ET1.getText().toString()) == 0)
                        TV1.setText(formattedDate + " 发送成功");
                    else
                        TV1.setText(formattedDate + " 发送失败");
                }

            }

        });

        // 处理读写权限
        if (!Environment.isExternalStorageManager()) {
            Intent intent = new Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION);
            Uri uri = Uri.fromParts("package", getPackageName(), null);
            intent.setData(uri);
            startActivity(intent);
        }





    }

    public native int FileSend(String ip, String port, String file);
    public native int DataSend(String ip, String port, String data);
}