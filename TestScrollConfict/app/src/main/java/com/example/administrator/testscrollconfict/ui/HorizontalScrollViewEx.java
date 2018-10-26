package com.example.administrator.testscrollconfict.ui;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.VelocityTracker;
import android.view.View;
import android.view.ViewConfiguration;
import android.view.ViewGroup;
import android.widget.Scroller;

public class HorizontalScrollViewEx extends ViewGroup {
    private static final String TAG = "HorizontalScrollViewEx";
    private static int TOUCH_SLOAP;
    // 子View的个数
    private int mChildrenSize;
    // 当前子View的宽度
    private int mChildWidth;
    // 当前子View的Index
    private int mChildIndex;

    // 分别记录上次滑动的坐标
    private int mLastX = 0;
    private int mLastY = 0;
    // 分别记录上次滑动的坐标(onInterceptTouchEvent)
    private int mLastXIntercept = 0;
    private int mLastYIntercept = 0;

    // 用于平滑滑动
    private Scroller mScroller;
    // 用于计算滑动速度
    private VelocityTracker mVelocityTracker;

    // 滑动方向
    private MOVE_DIECTION mMoveDirection;

    // 每次手指按下时的X坐标
    float startX = 0;
    float startY = 0;

    public enum MOVE_DIECTION{
        TOP,BOTTOM,LEFT,RIGHT
    }


    public HorizontalScrollViewEx(Context context) {
        super(context);
        init();
    }

    public HorizontalScrollViewEx(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public HorizontalScrollViewEx(Context context, AttributeSet attrs,
            int defStyle) {
        super(context, attrs, defStyle);
        init();
    }

    private void init() {
        mScroller = new Scroller(getContext());
        mVelocityTracker = VelocityTracker.obtain();
        TOUCH_SLOAP = ViewConfiguration.get(getContext()).getScaledTouchSlop();
    }

    @Override
    public boolean onInterceptTouchEvent(MotionEvent event) {
        boolean intercepted = false;
        int x = (int) event.getX();
        int y = (int) event.getY();

        switch (event.getAction()) {
        case MotionEvent.ACTION_DOWN: {
            intercepted = false;
            if (!mScroller.isFinished()) {
                mScroller.abortAnimation();
                // 此时正在水平滑动，如果这时候用户竖直滑动，防止界面处于中间状态，这里需要拦截事件
                intercepted = true;
            }

            startX = event.getX();
            startY = event.getY();

            break;
        }
        case MotionEvent.ACTION_MOVE: {
            int deltaX = x - mLastXIntercept;
            int deltaY = y - mLastYIntercept;

            // 判断是水平方向滑动还是竖直方向滑动
            if (Math.abs(deltaX) > Math.abs(deltaY)) {
                // 水平方向滑动则拦截事件，由HorizontalScrollViewEx处理事件
                intercepted = true;
            } else {
                // 竖直方向滑动则不拦截事件，由子View处理事件
                intercepted = false;
            }
            break;
        }
        case MotionEvent.ACTION_UP: {
            intercepted = false;
            break;
        }
        default:
            break;
        }

        Log.d(TAG, "intercepted=" + intercepted);
        mLastX = x;
        mLastY = y;
        mLastXIntercept = x;
        mLastYIntercept = y;

        return intercepted;
    }


    @Override
    public boolean onTouchEvent(MotionEvent event) {
        mVelocityTracker.addMovement(event);
        int x = (int) event.getX();
        int y = (int) event.getY();
        switch (event.getAction()) {
        case MotionEvent.ACTION_DOWN: {
            if (!mScroller.isFinished()) {
                mScroller.abortAnimation();
            }

            break;
        }
        case MotionEvent.ACTION_MOVE: {
            int deltaX = x - mLastX;
            int deltaY = y - mLastY;
            // 滑动过程中左右滑动
            scrollBy(-deltaX, 0);
            break;
        }
        case MotionEvent.ACTION_UP: {
            // 水平滑动距离
            int scrollX = getScrollX();
            // 计算滑动速度
            mVelocityTracker.computeCurrentVelocity(1000);
            float xVelocity = mVelocityTracker.getXVelocity();

            float curDx = Math.abs(startX - event.getX());

            Log.d(TAG, "onTouchEvent: xVelocity: " + xVelocity + " curDx: " + curDx);

            //获取到距离差
            float dx = x - startX;
            float dy = y - startY;
            //防止是按下也判断
            if (Math.abs(dx)> TOUCH_SLOAP || Math.abs(dy)> TOUCH_SLOAP) {
                //通过距离差判断方向
                int orientation = getOrientation(dx, dy);
                switch (orientation) {
                    case 'r':
                        mMoveDirection = MOVE_DIECTION.RIGHT;
                        Log.d(TAG, "MOVE_DIECTION: 向右滑动了");
                        break;
                    case 'l':
                        mMoveDirection = MOVE_DIECTION.LEFT;
                        Log.d(TAG, "MOVE_DIECTION: 向左滑动了");
                        break;
                    case 't':
                        mMoveDirection = MOVE_DIECTION.TOP;
                        Log.d(TAG, "MOVE_DIECTION: 向上滑动了");
                        break;
                    case 'b':
                        mMoveDirection = MOVE_DIECTION.BOTTOM;
                        Log.d(TAG, "MOVE_DIECTION: 向下滑动了");
                        break;
                }
            }

            // 滑动速度大于100并且水平滑动距离大于100的时候左右滑动
            if (Math.abs(xVelocity) >= 100 || curDx > mChildWidth/2) {
                if(mMoveDirection == MOVE_DIECTION.LEFT){
                    mChildIndex = mChildIndex + 1;
                }else if(mMoveDirection == MOVE_DIECTION.RIGHT){
                    mChildIndex = mChildIndex - 1;
                }
            } else {
                mChildIndex = (scrollX + mChildWidth / 2) / mChildWidth;
                Log.d(TAG, "onTouchEvent: mChildIndex: " + mChildIndex);
            }
            mChildIndex = Math.max(0, Math.min(mChildIndex, mChildrenSize - 1));
            int scrollDx = mChildIndex * mChildWidth - scrollX;
            smoothScrollBy(scrollDx, 0);
            mVelocityTracker.clear();
            break;
        }
        default:
            break;
        }

        mLastX = x;
        mLastY = y;
        return true;
    }


    /**
     * 根据距离差判断 滑动方向
     * @param dx X轴的距离差
     * @param dy Y轴的距离差
     * @return 滑动的方向
     */
    private int getOrientation(float dx, float dy) {
        if (Math.abs(dx)>Math.abs(dy)){
            //X轴移动
            return dx>0?'r':'l';
        }else{
            //Y轴移动
            return dy>0?'b':'t';
        }
    }


    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        int measuredWidth = 0;
        int measuredHeight = 0;
        final int childCount = getChildCount();
        measureChildren(widthMeasureSpec, heightMeasureSpec);

        int widthSpaceSize = MeasureSpec.getSize(widthMeasureSpec);
        int widthSpecMode = MeasureSpec.getMode(widthMeasureSpec);
        int heightSpaceSize = MeasureSpec.getSize(heightMeasureSpec);
        int heightSpecMode = MeasureSpec.getMode(heightMeasureSpec);
        if (childCount == 0) {
            setMeasuredDimension(0, 0);
        } else if (heightSpecMode == MeasureSpec.AT_MOST) {
            final View childView = getChildAt(0);
            measuredHeight = childView.getMeasuredHeight();
            setMeasuredDimension(widthSpaceSize, childView.getMeasuredHeight());
        } else if (widthSpecMode == MeasureSpec.AT_MOST) {
            final View childView = getChildAt(0);
            measuredWidth = childView.getMeasuredWidth() * childCount;
            setMeasuredDimension(measuredWidth, heightSpaceSize);
        } else {
            final View childView = getChildAt(0);
            measuredWidth = childView.getMeasuredWidth() * childCount;
            measuredHeight = childView.getMeasuredHeight();
            setMeasuredDimension(measuredWidth, measuredHeight);
        }
    }

    @Override
    protected void onLayout(boolean changed, int l, int t, int r, int b) {
        int childLeft = 0;
        final int childCount = getChildCount();
        mChildrenSize = childCount;

        for (int i = 0; i < childCount; i++) {
            final View childView = getChildAt(i);
            if (childView.getVisibility() != View.GONE) {
                final int childWidth = childView.getMeasuredWidth();
                mChildWidth = childWidth;
                childView.layout(childLeft, 0, childLeft + childWidth,
                        childView.getMeasuredHeight());
                childLeft += childWidth;
            }
        }
    }

    private void smoothScrollBy(int dx, int dy) {
        mScroller.startScroll(getScrollX(), 0, dx, 0, 500);
        invalidate();
    }

    @Override
    public void computeScroll() {
        if (mScroller.computeScrollOffset()) {
            scrollTo(mScroller.getCurrX(), mScroller.getCurrY());
            postInvalidate();
        }
    }

    @Override
    protected void onDetachedFromWindow() {
        mVelocityTracker.recycle();
        super.onDetachedFromWindow();
    }
}
