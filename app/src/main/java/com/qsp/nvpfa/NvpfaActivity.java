
package com.qsp.nvpfa;
import org.libsdl.app.SDLActivity;

import android.os.Build;
import android.os.Bundle;
import android.Manifest;
import android.content.pm.PackageManager;
import android.content.Intent;
import android.net.Uri;
import android.os.Environment;
import android.provider.Settings;

import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.annotation.NonNull;

public class NvpfaActivity extends SDLActivity {
    private static final int REQUEST_STORAGE_PERMISSION = 1;
    private static final int REQUEST_MANAGE_EXTERNAL_STORAGE = 2;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        getFilesDir();

        // 检查是否已经拥有权限
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED ||
            ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            // 如果没有权限，申请权限
            ActivityCompat.requestPermissions(this,
                new String[]{
                    Manifest.permission.READ_EXTERNAL_STORAGE,
                    Manifest.permission.WRITE_EXTERNAL_STORAGE
                },
                REQUEST_STORAGE_PERMISSION);
        } else {
            // 检查是否拥有 MANAGE_EXTERNAL_STORAGE 权限
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                if (!Environment.isExternalStorageManager()) {
                    // 如果没有该权限，引导用户手动开启
                    Intent intent = new Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION);
                    intent.setData(Uri.parse("package:" + getPackageName()));
                    startActivityForResult(intent, REQUEST_MANAGE_EXTERNAL_STORAGE);
                } else {
                    // 如果已经拥有所有权限，直接初始化应用
                    initializeApp();
                }
            }
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == REQUEST_STORAGE_PERMISSION) {
            if (grantResults.length > 0 &&
                grantResults[0] == PackageManager.PERMISSION_GRANTED &&
                grantResults[1] == PackageManager.PERMISSION_GRANTED) {
                // 权限申请成功，检查 MANAGE_EXTERNAL_STORAGE 权限
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                    if (!Environment.isExternalStorageManager()) {
                        // 如果没有该权限，引导用户手动开启
                        Intent intent = new Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION);
                        intent.setData(Uri.parse("package:" + getPackageName()));
                        startActivityForResult(intent, REQUEST_MANAGE_EXTERNAL_STORAGE);
                    } else {
                        // 如果已经拥有所有权限，直接初始化应用
                        initializeApp();
                    }
                }
            } else {
                // 权限申请失败，提示用户
                System.out.println("Storage permissions were not granted.");
            }
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == REQUEST_MANAGE_EXTERNAL_STORAGE) {
            if (Environment.isExternalStorageManager()) {
                // 用户已经手动开启了 MANAGE_EXTERNAL_STORAGE 权限
                initializeApp();
            } else {
                // 用户拒绝了权限，提示用户
                System.out.println("MANAGE_EXTERNAL_STORAGE permission was not granted.");
            }
        }
    }

    private void initializeApp() {
        // 在这里执行应用的初始化逻辑
        System.out.println("NvpfaActivity initialized with storage permissions.");
    }
}
