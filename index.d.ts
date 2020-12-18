interface WeWorkConfig {
  corpid: string;
  secret: string;
  pk: string;
}

interface MediaBody {
  /**
   * 媒体资源的id信息。String类型
   */
  sdkfileid: string;

  /**
   * 资源的md5值，供进行校验。String类型
   */
  md5sum: string;

  /**
   * 资源的文件大小。Uint32类型
   */
  filesize: string;

}

interface VoiceBody extends Omit<MediaBody, "filesize"> {
  /**
   * 语音消息大小。Uint32类型
   */
  voice_size: number;

  /**
   * 播放长度。Uint32类型
   */
  play_length: number;
}

interface EmotionBody extends Omit<MediaBody, "filesize"> {
  /**
   * 表情类型，png或者gif.1表示gif 2表示png。Uint32类型
   */
  type: 1 | 2;

  /**
   * 表情图片宽度。Uint32类型
   */
  width: number;

  /**
   * 表情图片高度。Uint32类型
   */
  height: number;

  /**
   * 资源的文件大小。Uint32类型
   */
  imagesize: number;
}

interface VideoBody extends MediaBody {
  /**
   * 视频播放长度。Uint32类型
   */
  play_length: number;
}

interface FileBody extends MediaBody {
  /**
   * 文件名称。String类型
   */
  filename: string;

  /**
   * 文件类型后缀。String类型
   */
  fileext: string;
}

interface ChatrecordBody {
  /**
   * 聊天记录标题。String类型
   */
  title: string;

  /**
   * 消息记录内的消息内容，批量数据
   * see: https://open.work.weixin.qq.com/api/doc/90000/90135/91774#%E6%B6%88%E6%81%AF%E6%A0%BC%E5%BC%8F
   */
  item: Array<{
    type: "text";
    msgtime: number;
    content: string;
    from_chatroom: boolean;
  }>;
}

interface RawChatData {
  seq: number;
  msgid: string;
  publickey_ver: number;
  encrypt_random_key: string;
  encrypt_chat_msg: string;
}

interface ChatData {
  /**
   * 消息标志位
   */
  seq: number;

  /**
   * 原消息体
   */
  raw: RawChatData;
  /**
   * 消息id，消息的唯一标识，企业可以使用此字段进行消息去重。String类型
   */
  msgid: string;

  /**
   * 消息动作，目前有send(发送消息)/recall(撤回消息)/switch(切换企业日志)三种类型。String类型
   */
  action: "send" | "recall" | "switch";

  /**
   * 消息发送方id。同一企业内容为userid，非相同企业为external_userid。消息如果是机器人发出，也为external_userid。String类型
   */
  from: string;

  /**
   * 消息接收方列表，可能是多个，同一个企业内容为userid，非相同企业为external_userid。数组，内容为string类型
   */
  tolist: string[];

  /**
   * 群聊消息的群id。如果是单聊则为空。String类型
   */
  roomid: string;

  /**
   * 消息发送时间戳，utc时间，ms单位。
   */
  msgtime: number;

  /**
   * 消息类型（String类型）:
   *   - 文本消息为：text
   *   - 图片消息为：image
   *   - 撤回消息为：revoke
   *   - 同意消息为：agree
   *   - 不同意消息为：disagree
   *   - 语音消息为：voice
   *   - 视频消息为：video
   *   - 名片消息为：card
   *   - 位置消息为：location
   *   - 表情消息为：emotion
   *   - 文件消息为：file
   *   - 链接消息为：link
   *   - 小程序消息为：weapp
   *   - 会话记录消息为：chatrecord
   *   - 待办消息为：todo
   *   - 投票消息为：vote
   *   - 填表消息为：collect
   *   - 红包消息为：redpacket
   *   - 会议邀请消息为：meeting
   *   - 在线文档消息为：docmsg
   *   - MarkDown消息为：markdown
   *   - 图文消息为：news
   *   - 日程消息为：calendar
   *   - 混合消息为：mixed
   *   - 音频存档消息为：meeting_voice_call
   *   - 互通红包消息为：external_redpacket
   */
  msgtype:
    | "text"
    | "image"
    | "revoke"
    | "agree"
    | "disagree"
    | "voice"
    | "video"
    | "card"
    | "location"
    | "emotion"
    | "file"
    | "link"
    | "weapp"
    | "chatrecord"
    | "todo"
    | "vote"
    | "collect"
    | "redpacket"
    | "meeting"
    | "docmsg"
    | "markdown"
    | "news"
    | "calendar"
    | "mixed"
    | "meeting_voice_call"
    | "external_redpacket";

  /**
   * 文本消息内容
   */
  content?: string;

  /**
   * 图片消息内容
   */
  image?: MediaBody;

  /**
   * 撤回消息内容
   */
  revoke?: {
    /**
     * 标识撤回的原消息的msgid
     */
    pre_msgid: string;
  };

  /**
   * 同意会话聊天内容
   */
  agree?: {
    /**
     * 同意协议者的userid，外部企业默认为external_userid。String类型
     */
    userid: string;

    /**
     * 同意协议的时间，utc时间，ms单位。
     */
    agree_time: number;
  };

  /**
   * 不同意会话聊天内容
   */
  disagree?: {
    /**
     * 不同意协议者的userid，外部企业默认为external_userid。String类型
     */
    userid: string;

    /**
     * 不同意协议的时间，utc时间，ms单位。
     */
    disagree_time: number;
  };

  /**
   * 语音消息
   */
  voice?: VoiceBody;

  /**
   * 视频消息
   */
  video?: VideoBody;

  /**
   * 名片消息
   */
  card?: {
    /**
     * 名片所有者所在的公司名称。String类型
     */
    corpname: string;

    /**
     * 名片所有者的id，同一公司是userid，不同公司是external_userid。String类型
     */
    userid: string;
  };

  /**
   * 位置消息
   */
  location?: {
    /**
     * 经度，单位double
     */
    longitude: number;

    /**
     * 纬度，单位double
     */
    latitude: number;

    /**
     * 地址信息。String类型
     */
    address: string;

    /**
     * 位置信息的title。String类型
     */
    title: string;

    /**
     * 缩放比例。Uint32类型
     */
    zoom: number;
  };

  /**
   * 表情消息
   */
  emotion?: EmotionBody;

  /**
   * 文件消息
   */
  file?: FileBody;

  /**
   * 链接消息
   */
  link?: {
    /**
     * 消息标题。String类型
     */
    title: string;

    /**
     * 消息描述。String类型
     */
    description: string;

    /**
     * 链接url地址。String类型
     */
    link_url: string;

    /**
     * 链接图片url。String类型
     */
    image_url: string;
  };

  /**
   * 小程序消息
   */
  weapp?: {
    /**
     * 消息标题。String类型
     */
    title: string;

    /**
     * 消息描述。String类型
     */
    description: string;

    /**
     * 用户名称。String类型
     */
    username: string;

    /**
     * 小程序名称。String类型
     */
    displayname: string;
  };

  /**
   * 会话记录消息
   */
  chatrecord?: ChatrecordBody;

  /**
   * 待办消息
   */
  todo?: {
    title: string;
    content: string;
  };

  /**
   * 投票消息
   */
  vote?: {
    votetitle: string;
    voteitem: string[];
    votetype: 101 | 102;
    voteid: string;
  };

  /**
   * 填表消息
   */
  collect?: {
    room_name: string;
    creator: string;
    create_time: string;
    title: string;
    details: Array<{ id: number; ques: string; type: string }>;
  };

  /**
   * 红包消息
   */
  redpacket?: {
    type: 1 | 2 | 3;
    wish: string;
    totalcnt: number;
    totalamount: number;
  };

  meeting?: any;
  docmsg?: any;
  markdown?: any;
  calendar?: any;
  mixed?: any;
  [k: string]: any;
}

declare class WeWork {
  constructor(config: WeWorkConfig);
  getChatData(seq: number, limit: number, timeout: number): RawChatData[];
  getDecryptedChatData(seq: number, limit: number, timeout: number): ChatData[];
  decryptData(msg: ChatData): ChatData;
  getMediaData(id: string, savedFilepath: string): string;
}

export = WeWork;
