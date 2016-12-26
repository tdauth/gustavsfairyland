package gustavsfairyland;

import java.io.*;
import android.content.*;
import android.media.*;
import android.widget.*;
import android.view.View;
import android.app.Activity;

public class Player extends View {
	private VideoView videoView;

	public Player(final Activity activity)
	{
		super(activity);
		this.videoView = new VideoView(this.getContext());
	}

	public void start(String filePath) {
		//videoView.setVideoPath(filePath);
		//videoView.start();
	}
}