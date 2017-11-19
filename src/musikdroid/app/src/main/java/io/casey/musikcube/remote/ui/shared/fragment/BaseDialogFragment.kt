package io.casey.musikcube.remote.ui.shared.fragment

import android.content.Intent
import android.os.Bundle
import android.support.v4.app.DialogFragment
import android.support.v7.app.AppCompatActivity
import io.casey.musikcube.remote.framework.IMixin
import io.casey.musikcube.remote.framework.MixinSet
import io.casey.musikcube.remote.framework.ViewModel
import io.casey.musikcube.remote.ui.shared.mixin.ItemContextMenuMixin
import io.casey.musikcube.remote.ui.shared.mixin.ViewModelMixin

open class BaseDialogFragment: DialogFragment(), ViewModel.Provider {
    private val mixins = MixinSet()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        mixins.onCreate(savedInstanceState ?: Bundle())
    }

    override fun onStart() {
        super.onStart()
        mixins.onStart()
    }

    override fun onResume() {
        super.onResume()
        mixins.onResume()
    }

    override fun onPause() {
        super.onPause()
        mixins.onPause()
    }

    override fun onStop() {
        super.onStop()
        mixins.onStop()
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        mixins.onActivityResult(requestCode, resultCode, data)
    }

    override fun onSaveInstanceState(outState: Bundle?) {
        super.onSaveInstanceState(outState)
        mixins.onSaveInstanceState(outState ?: Bundle())
    }

    override fun onDestroy() {
        super.onDestroy()
        mixins.onDestroy()
    }

    override fun <T: ViewModel<*>> createViewModel(): T? = null
    protected fun <T: ViewModel<*>> getViewModel(): T? = mixin(ViewModelMixin::class.java)?.get<T>() as T
    protected fun <T: IMixin> mixin(mixin: T): T = mixins.add(mixin)
    protected fun <T: IMixin> mixin(cls: Class<out T>): T? = mixins.get(cls)

    companion object {
        @Suppress("UNCHECKED_CAST")
        fun <T: BaseDialogFragment> find(activity: AppCompatActivity, tag: String): T? =
            activity.supportFragmentManager.findFragmentByTag(tag) as T?

        fun dismiss(activity: AppCompatActivity, tag: String) {
            find<BaseDialogFragment>(activity, tag)?.dismiss()
        }
    }

}